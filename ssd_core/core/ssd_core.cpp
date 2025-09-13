#include "ssd_core.h"

#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <cstring>

struct SSDHandle {
    int N;
    int current;
    std::vector<double> kappa; /* N*N */
    std::vector<double> w;     /* N*N */
    double E, F, T;
    std::vector<double> pi;    /* size N */
    SSDParams prm;
    std::mt19937_64 rng;
    std::normal_distribution<double> norm01;
    std::uniform_real_distribution<double> uni01;

    SSDHandle(int n, const SSDParams& p, uint64_t seed)
        : N(n), current(0), kappa(n*n, 0.0), w(n*n, 0.0),
          E(0.0), F(0.0), T(p.T0), pi(n, 1.0 / std::max(1,n)),
          prm(p), rng(seed), norm01(0.0, 1.0), uni01(0.0, 1.0) {}
};

/* --- ヘルパー関数 --- */
static inline double clip(double x, double lo, double hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

static inline int idx(int i, int j, int N) { 
    return i * N + j; 
}

static void softmax_temp(const std::vector<double>& logits, double T, std::vector<double>& out) {
    int n = (int)logits.size();
    out.resize(n);
    if (n == 0) return;
    
    if (T <= 1e-8) {
        // argmax one-hot
        int arg = 0;
        for (int i = 1; i < n; ++i) {
            if (logits[i] > logits[arg]) arg = i;
        }
        std::fill(out.begin(), out.end(), 0.0);
        out[arg] = 1.0;
        return;
    }
    
    // 数値安定化のため最大値を引く
    double maxv = *std::max_element(logits.begin(), logits.end());
    
    // exp計算と正規化
    double sum = 0.0;
    for (int i = 0; i < n; ++i) {
        double z = (logits[i] - maxv) / T;
        double e = std::exp(z);
        out[i] = e;
        sum += e;
    }
    
    if (sum <= 0.0) sum = 1.0;
    for (int i = 0; i < n; ++i) {
        out[i] /= sum;
    }
}

static double entropy_norm(const std::vector<double>& p) {
    int n = (int)p.size();
    if (n == 0) return 0.0;
    
    double H = 0.0;
    for (int i = 0; i < n; ++i) {
        double x = p[i] <= 1e-12 ? 1e-12 : p[i];
        H += -x * std::log(x);
    }
    
    double Hmax = std::log((double)n);
    return Hmax > 0.0 ? (H / Hmax) : 0.0;
}

/* --- API実装 --- */

extern "C" SSDHandle* ssd_create(int32_t N, const SSDParams* params, uint64_t seed) {
    if (N <= 0) return nullptr;
    
    SSDParams p;
    if (params) {
        std::memcpy(&p, params, sizeof(SSDParams));
    } else {
        // デフォルトパラメータ（構造体初期化で既に設定済み）
    }
    
    if (seed == 0) seed = 123456789ULL;
    
    try {
        return new SSDHandle(N, p, seed);
    } catch (...) {
        return nullptr;
    }
}

extern "C" void ssd_destroy(SSDHandle* h) {
    delete h;
}

extern "C" void ssd_get_params(SSDHandle* h, SSDParams* out) {
    if (!h || !out) return;
    std::memcpy(out, &h->prm, sizeof(SSDParams));
}

extern "C" void ssd_set_params(SSDHandle* h, const SSDParams* in) {
    if (!h || !in) return;
    std::memcpy(&h->prm, in, sizeof(SSDParams));
}

extern "C" int32_t ssd_get_N(SSDHandle* h) {
    return h ? h->N : 0;
}

extern "C" int32_t ssd_get_kappa_row(SSDHandle* h, int32_t row, double* out_buf, int32_t len) {
    if (!h || !out_buf || row < 0 || row >= h->N) return 0;
    
    int N = h->N;
    int m = std::min(N, (int)len);
    std::memcpy(out_buf, h->kappa.data() + row * N, sizeof(double) * m);
    return m;
}

extern "C" void ssd_step(SSDHandle* h, double p, double dt, SSDTelemetry* out) {
    if (!h) return;
    
    int N = h->N;
    auto& prm = h->prm;

    // === 1. AlignFlow（整合流計算） ===
    // j = (G0 + g*kappa) * p + noise
    std::vector<double> j(N * N, 0.0);
    double J_norm = 0.0;
    
    for (int i = 0; i < N * N; ++i) {
        double val = (prm.G0 + prm.g * h->kappa[i]) * p;
        
        // ノイズ追加（オプション）
        if (prm.eps_noise > 0.0) {
            val += prm.eps_noise * h->norm01(h->rng);
        }
        
        j[i] = val;
        J_norm += val * val;
    }
    J_norm = std::sqrt(J_norm);

    // === 2. UpdateKappa（整合慣性更新） ===
    for (int i = 0; i < N * N; ++i) {
        // 整合仕事: p*j - rho*j^2
        double align_work = p * j[i] - prm.rho * j[i] * j[i];
        double gain = prm.eta * align_work;
        double decay = prm.lam * (h->kappa[i] - prm.kappa_min);
        
        double new_kappa = h->kappa[i] + (gain - decay) * dt;
        h->kappa[i] = std::max(new_kappa, prm.kappa_min);
    }

    // === 3. UpdateHeat（熱蓄積更新） ===
    double excess_pressure = std::max(std::abs(p) - J_norm, 0.0);
    double dE = prm.alpha * excess_pressure - prm.beta_E * h->E;
    h->E = std::max(h->E + dE * dt, 0.0);

    // === 4. Threshold / JumpRate / Temperature ===
    // 平均慣性計算
    double kappa_mean = 0.0;
    for (double k : h->kappa) kappa_mean += k;
    kappa_mean /= (double)(N * N);

    // 動的閾値
    double Theta = prm.Theta0 + prm.a1 * kappa_mean - prm.a2 * h->F;
    
    // 跳躍率（ポアソン過程）
    double hrate = prm.h0 * std::exp((h->E - Theta) / std::max(1e-8, prm.gamma));
    
    // 探索温度（硬直検知付き）
    double policy_entropy = h->pi.empty() ? 1.0 : entropy_norm(h->pi);
    h->T = std::max(1e-6, prm.T0 + prm.c1 * h->E - prm.c2 * policy_entropy);

    // === 5. 跳躍判定と実行 ===
    bool did_jump = false;
    int rewired_to = h->current;

    double jump_probability = 1.0 - std::exp(-hrate * dt);
    if (h->uni01(h->rng) < jump_probability) {
        did_jump = true;
        
        // === 制約付きランダム接続 ===
        // 現在ノードから他のノードへの接続確率を計算
        std::vector<double> logits(N, 0.0);
        for (int k = 0; k < N; ++k) {
            // 既存の慣性をベースとする
            logits[k] = h->kappa[idx(h->current, k, N)];
            
            // 自己接続を抑制
            if (k == h->current) {
                logits[k] -= 1.0;
            }
            
            // ガウスノイズ追加
            logits[k] += prm.sigma * h->norm01(h->rng);
        }
        
        // ソフトマックスで確率分布を計算
        softmax_temp(logits, h->T, h->pi);
        
        // カテゴリカル分布からサンプリング
        double r = h->uni01(h->rng);
        double cdf = 0.0;
        int selected = N - 1;  // フォールバック
        for (int k = 0; k < N; ++k) {
            cdf += h->pi[k];
            if (r <= cdf) {
                selected = k;
                break;
            }
        }
        
        // === Rewire（再配線） ===
        int edge_idx = idx(h->current, selected, N);
        h->w[edge_idx] += prm.delta_w;
        h->kappa[edge_idx] += prm.delta_kappa;
        
        // 放熱
        h->E *= prm.c0_cool;
        
        // 位置更新
        h->current = selected;
        rewired_to = selected;

        // === RelaxTop（硬直ほぐし） ===
        int total_edges = N * N;
        int relax_count = std::max(1, (int)std::round(prm.q_relax * total_edges));
        
        // 流量の絶対値でソート用インデックスを作成
        std::vector<int> indices(total_edges);
        for (int i = 0; i < total_edges; ++i) indices[i] = i;
        
        std::partial_sort(indices.begin(), indices.begin() + relax_count, indices.end(),
            [&](int a, int b) { return std::abs(j[a]) > std::abs(j[b]); });
        
        // 上位q%の経路を微緩和
        for (int i = 0; i < relax_count; ++i) {
            int pos = indices[i];
            double new_kappa = h->kappa[pos] - prm.eps_relax;
            h->kappa[pos] = std::max(new_kappa, prm.kappa_min);
        }
        
    } else {
        // === ε-greedy完全ランダム探索 ===
        double eps = prm.eps0 + prm.d1 * h->E - prm.d2 * kappa_mean;
        eps = clip(eps, 0.0, 1.0);
        
        if (h->uni01(h->rng) < eps) {
            int k = (int)std::floor(h->uni01(h->rng) * N);
            if (k == N) k = N - 1;
            
            if (k != h->current) {
                int edge_idx = idx(h->current, k, N);
                h->w[edge_idx] += 0.05;  // 小さな重み追加
                h->kappa[edge_idx] += 0.05;  // 小さな慣性追加
            }
        }
        
        // === 決定論的行動選択 ===
        // 現在位置から最も慣性の高い接続先を選択
        int best = h->current;
        double best_value = -1e300;
        
        for (int k = 0; k < N; ++k) {
            double value = h->kappa[idx(h->current, k, N)];
            if (k == h->current) value -= 1e-6;  // 自己接続を微妙に抑制
            
            if (value > best_value) {
                best_value = value;
                best = k;
            }
        }
        
        h->current = best;
        rewired_to = best;
    }

    // === 6. テレメトリ出力 ===
    if (out) {
        double align_efficiency = (std::abs(p) > 1e-8) ? (J_norm / std::abs(p)) : 0.0;
        
        out->E = h->E;
        out->Theta = Theta;
        out->h = hrate;
        out->T = h->T;
        out->H = policy_entropy;
        out->J_norm = J_norm;
        out->align_eff = align_efficiency;
        out->kappa_mean = kappa_mean;
        out->current = h->current;
        out->did_jump = did_jump ? 1 : 0;
        out->rewired_to = rewired_to;
    }
}