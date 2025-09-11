/*
 * ssd_universal_engine_dll.cpp
 * 構造主観力学（SSD）汎用評価エンジン実装 - 修正版
 */

#define SSD_UNIVERSAL_DLL_EXPORTS
#include "ssd_universal_engine_dll.h"

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <random>
#include <memory>
#include <cstring>
#include <chrono>
#include <unordered_map>
#include <stdexcept>
#include <mutex>

/* ========================================
 * 内部データ構造
 * ======================================== */

struct DomainCoefficients {
    double structure_weight;
    double pressure_weight;
    double alignment_weight;
    double jump_weight;
    double time_scale_factor;
    double space_scale_factor;
};

struct CacheEntry {
    SSDUniversalEvaluationResult result;
    std::chrono::steady_clock::time_point timestamp;
    size_t hash_key;
};

class SSDUniversalEngine {
public:
    SSDEngineConfig config;
    std::string engine_id;
    std::string version;
    std::chrono::steady_clock::time_point start_time;
    
    // 共有資源用ミューテックス
    std::mutex cache_mutex;
    std::mutex stats_mutex;
    
    // 統計情報
    uint64_t total_evaluations;
    double total_computation_time;
    uint64_t cache_hits;
    std::vector<double> recent_accuracy_scores;
    
    // キャッシュ
    std::unordered_map<size_t, CacheEntry> cache;
    
    // ドメイン特化係数
    std::unordered_map<SSDDomain, DomainCoefficients> domain_coefficients;
    
    // エラーメッセージ
    std::string last_error;
    
    // ストリーミング
    SSDStreamingCallback streaming_callback;
    void* streaming_user_data;
    bool streaming_active;
    
    // 乱数生成器
    std::mt19937 rng;
    
    SSDUniversalEngine(const SSDEngineConfig* config);
    ~SSDUniversalEngine();
    
    SSDReturnCode evaluate_system(
        const SSDUniversalStructure* structures, int32_t structure_count,
        const SSDUniversalMeaningPressure* pressures, int32_t pressure_count,
        const SSDEvaluationContext* context,
        SSDUniversalEvaluationResult* result);
    
    SSDReturnCode calculate_inertia_unified(
        SSDStructureLayer layer, SSDInertiaType type,
        const SSDInertiaComponent* components, int32_t count,
        const SSDEvaluationContext* context,
        double* out_inertia, double* out_confidence);
    
private:
    void initialize_domain_coefficients();
    void update_statistics(double computation_time, double confidence);
    size_t calculate_hash(const SSDUniversalStructure* structures, int32_t structure_count,
                         const SSDUniversalMeaningPressure* pressures, int32_t pressure_count,
                         const SSDEvaluationContext* context);
    
    // 分析関数
    void analyze_structures(const SSDUniversalStructure* structures, int32_t count,
                           const SSDEvaluationContext* context, const DomainCoefficients& coeff,
                           double& stability, double& complexity, double& adaptability);
    
    void analyze_pressures(const SSDUniversalMeaningPressure* pressures, int32_t count,
                          const SSDEvaluationContext* context, const DomainCoefficients& coeff,
                          double& magnitude, double& coherence, double& sustainability);
    
    void analyze_alignment(const SSDUniversalStructure* structures, int32_t structure_count,
                          const SSDUniversalMeaningPressure* pressures, int32_t pressure_count,
                          const SSDEvaluationContext* context, const DomainCoefficients& coeff,
                          double& strength, double& efficiency, double& durability);
    
    void analyze_jump_potential(const SSDUniversalStructure* structures, int32_t structure_count,
                               const SSDUniversalMeaningPressure* pressures, int32_t pressure_count,
                               const SSDEvaluationContext* context, const DomainCoefficients& coeff,
                               double& probability, double* direction, int32_t& direction_dims, double& impact);
    
    void integrate_analyses(double struct_stability, double struct_complexity, double struct_adaptability,
                           double press_magnitude, double press_coherence, double press_sustainability,
                           double align_strength, double align_efficiency, double align_durability,
                           double jump_probability, double jump_impact,
                           double& system_health, double& evolution_potential, double& stability_resilience);
    
    void generate_warnings_and_recommendations(const SSDUniversalEvaluationResult& result,
                                              uint32_t& warnings, uint32_t& recommendations);
    
    double calculate_confidence(const SSDUniversalStructure* structures, int32_t structure_count,
                               const SSDUniversalMeaningPressure* pressures, int32_t pressure_count,
                               const SSDEvaluationContext* context);
};

/* ========================================
 * エンジン実装
 * ======================================== */

SSDUniversalEngine::SSDUniversalEngine(const SSDEngineConfig* cfg) 
    : version("1.0.0"), total_evaluations(0), total_computation_time(0.0),
      cache_hits(0), streaming_callback(nullptr), streaming_user_data(nullptr),
      streaming_active(false), rng(std::random_device{}())
{
    start_time = std::chrono::steady_clock::now();
    
    // デフォルト設定またはユーザー設定を適用
    if (cfg) {
        this->config = *cfg;
    } else {
        // デフォルト設定
        this->config.precision_level = 2;  // high
        this->config.calculation_mode = 1; // balanced
        this->config.enable_cache = 1;
        this->config.enable_prediction = 1;
        this->config.enable_explanation = 1;
        this->config.max_iterations = 1000;
        this->config.convergence_threshold = 1e-6;
        this->config.time_limit_ms = 5000;
        this->config.parallel_processing = 1;
        this->config.memory_limit_mb = 512;
        
        // デフォルト重み設定
        for (int i = 0; i < 8; i++) {
            this->config.domain_weights[i] = 1.0;
        }
        this->config.layer_weights[0] = 1.0; // physical
        this->config.layer_weights[1] = 0.9; // basal
        this->config.layer_weights[2] = 0.7; // core
        this->config.layer_weights[3] = 0.5; // upper
    }
    
    // エンジンID生成
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        start_time.time_since_epoch()).count();
    engine_id = "ssd_engine_" + std::to_string(timestamp);
    
    initialize_domain_coefficients();
}

SSDUniversalEngine::~SSDUniversalEngine() {
    // デストラクタ - 自動的にクリーンアップ
}

void SSDUniversalEngine::initialize_domain_coefficients() {
    // 物理学ドメイン
    domain_coefficients[SSD_DOMAIN_PHYSICS] = {
        1.0, 1.0, 0.9, 0.8, 1.0, 1.0
    };
    
    // 化学ドメイン
    domain_coefficients[SSD_DOMAIN_CHEMISTRY] = {
        0.9, 1.0, 1.0, 0.9, 1e3, 1e-10
    };
    
    // 生物学ドメイン
    domain_coefficients[SSD_DOMAIN_BIOLOGY] = {
        0.8, 0.9, 1.0, 1.0, 1e6, 1e-6
    };
    
    // 心理学ドメイン
    domain_coefficients[SSD_DOMAIN_PSYCHOLOGY] = {
        0.7, 1.0, 0.8, 1.0, 1e0, 1e0
    };
    
    // 社会学ドメイン
    domain_coefficients[SSD_DOMAIN_SOCIOLOGY] = {
        0.6, 0.8, 0.9, 1.0, 1e7, 1e3
    };
    
    // 経済学ドメイン
    domain_coefficients[SSD_DOMAIN_ECONOMICS] = {
        0.5, 1.0, 0.7, 1.0, 1e6, 1e6
    };
    
    // AIドメイン
    domain_coefficients[SSD_DOMAIN_AI] = {
        0.8, 0.9, 1.0, 0.9, 1e-3, 1e0
    };
}

SSDReturnCode SSDUniversalEngine::evaluate_system(
    const SSDUniversalStructure* structures, int32_t structure_count,
    const SSDUniversalMeaningPressure* pressures, int32_t pressure_count,
    const SSDEvaluationContext* context,
    SSDUniversalEvaluationResult* result)
{
    auto calc_start_time = std::chrono::high_resolution_clock::now();
    
    // 入力検証
    if (!structures || structure_count <= 0 || !pressures || pressure_count <= 0 || 
        !context || !result) {
        last_error = "Invalid input parameters";
        return SSD_ERROR_INVALID_INPUT;
    }
    
    try {
        // ハッシュ計算とキャッシュ確認
        size_t hash_key = 0;
        if (config.enable_cache) {
            hash_key = calculate_hash(structures, structure_count, pressures, pressure_count, context);
            std::lock_guard<std::mutex> lock(cache_mutex);
            auto cache_it = cache.find(hash_key);
            if (cache_it != cache.end()) {
                *result = cache_it->second.result;
                strncpy(result->evaluation_id, engine_id.c_str(), sizeof(result->evaluation_id) - 1);
                result->evaluation_id[sizeof(result->evaluation_id) - 1] = '\0';
                cache_hits++;
                return SSD_SUCCESS;
            }
        }
        
        // 結果構造体初期化
        memset(result, 0, sizeof(SSDUniversalEvaluationResult));
        strncpy(result->evaluation_id, engine_id.c_str(), sizeof(result->evaluation_id) - 1);
                result->evaluation_id[sizeof(result->evaluation_id) - 1] = '\0';
        
        // ドメイン係数取得
        auto coeff_it = domain_coefficients.find(context->domain);
        DomainCoefficients coeff = (coeff_it != domain_coefficients.end()) ? 
            coeff_it->second : domain_coefficients[SSD_DOMAIN_PHYSICS];
        
        // 1. 構造分析
        analyze_structures(structures, structure_count, context, coeff,
                          result->structure_stability, result->structure_complexity, 
                          result->structure_adaptability);
        
        // 2. 意味圧分析
        analyze_pressures(pressures, pressure_count, context, coeff,
                         result->pressure_magnitude, result->pressure_coherence, 
                         result->pressure_sustainability);
        
        // 3. 整合分析
        analyze_alignment(structures, structure_count, pressures, pressure_count, context, coeff,
                         result->alignment_strength, result->alignment_efficiency, 
                         result->alignment_durability);
        
        // 4. 跳躍分析
        analyze_jump_potential(structures, structure_count, pressures, pressure_count, context, coeff,
                              result->jump_probability, result->jump_direction, 
                              result->jump_direction_dims, result->jump_impact_estimation);
        
        // 5. 統合分析
        integrate_analyses(
            result->structure_stability, result->structure_complexity, result->structure_adaptability,
            result->pressure_magnitude, result->pressure_coherence, result->pressure_sustainability,
            result->alignment_strength, result->alignment_efficiency, result->alignment_durability,
            result->jump_probability, result->jump_impact_estimation,
            result->system_health, result->evolution_potential, result->stability_resilience
        );
        
        // 6. 信頼度計算
        result->calculation_confidence = calculate_confidence(structures, structure_count, 
                                                              pressures, pressure_count, context);
        
        // 7. 計算時間記録
        auto calc_end_time = std::chrono::high_resolution_clock::now();
        result->computational_cost = std::chrono::duration<double>(calc_end_time - calc_start_time).count();
        
        // 8. 予測期間推定
        static const double scale_factors[] = {1e-15, 1e-12, 1e-9, 1e-3, 1e3, 1e6, 1e9, 1e12};
        int sl = std::clamp(static_cast<int>(context->scale_level), 0, 7);
        result->prediction_horizon = scale_factors[sl] * context->time_scale * coeff.time_scale_factor;
        
        // 9. 警告・推奨生成
        generate_warnings_and_recommendations(*result, result->warning_flags, 
                                              result->recommendation_flags);
        
        // 10. 説明JSON生成
        snprintf(result->explanation_json, sizeof(result->explanation_json),
                "{\n"
                "  \"structure_factors\": {\"stability\":%.3f, \"complexity\":%.3f, \"adaptability\":%.3f},\n"
                "  \"pressure_factors\": {\"magnitude\":%.3f, \"coherence\":%.3f, \"sustainability\":%.3f},\n"
                "  \"integration\": {\"health_formula\":\"0.3*stability+0.3*alignment+0.2*efficiency+0.2*(1-jump)\",\n"
                "                   \"domain\":\"%d\", \"scale\":\"%d\"}\n"
                "}",
                result->structure_stability, result->structure_complexity, result->structure_adaptability,
                result->pressure_magnitude, result->pressure_coherence, result->pressure_sustainability,
                (int)context->domain, (int)context->scale_level);
        
        // 11. キャッシュ保存
        if (config.enable_cache && cache.size() < 1000) {
            CacheEntry entry;
            entry.result = *result;
            entry.timestamp = std::chrono::steady_clock::now();
            entry.hash_key = hash_key;
            {
            std::lock_guard<std::mutex> lock(cache_mutex);
            cache[hash_key] = entry;
        }
        }
        
        // 12. 統計更新
        update_statistics(result->computational_cost, result->calculation_confidence);
        
        result->return_code = (result->calculation_confidence < 0.3) ? 
            SSD_WARNING_LOW_CONFIDENCE : SSD_SUCCESS;
        return result->return_code;
        
    } catch (const std::exception& e) {
        last_error = std::string("Calculation failed: ") + e.what();
        return SSD_ERROR_CALCULATION_FAILED;
    }
}

void SSDUniversalEngine::analyze_structures(
    const SSDUniversalStructure* structures, int32_t count,
    const SSDEvaluationContext* context, const DomainCoefficients& coeff,
    double& stability, double& complexity, double& adaptability)
{
    if (count == 0) {
        stability = complexity = adaptability = 0.0;
        return;
    }
    
    double total_stability = 0.0;
    double total_complexity = 0.0;
    double total_adaptability = 0.0;
    
    for (int32_t i = 0; i < count; i++) {
        const auto& s = structures[i];
        
        // 安定性計算
        double struct_stability = s.stability_index;
        
        // 制約行列による補正
        if (s.constraint_rows > 0 && s.constraint_cols > 0) {
            double constraint_sum = 0.0;
            int elements = s.constraint_rows * s.constraint_cols;
            for (int j = 0; j < std::min(elements, 16); j++) {
                constraint_sum += s.constraint_matrix[j];
            }
            double constraint_effect = constraint_sum / elements;
            struct_stability *= (1.0 + constraint_effect * 0.2);
        }
        
        // ドメイン補正
        struct_stability *= coeff.structure_weight;
        
        total_stability += std::max(0.0, std::min(1.0, struct_stability));
        
        // 複雑性計算
        double struct_complexity = s.complexity_level;
        double dimension_factor = 1.0 + std::log(std::max(1, s.dimension_count)) * 0.1;
        struct_complexity *= dimension_factor;
        
        // 動的特性による補正
        if (s.dynamic_count > 0) {
            double dynamics_sum = 0.0;
            for (int j = 0; j < s.dynamic_count && j < 16; j++) {
                dynamics_sum += s.dynamic_properties[j];
            }
            double dynamics_factor = dynamics_sum / s.dynamic_count;
            struct_complexity *= (1.0 + dynamics_factor * 0.3);
        }
        
        total_complexity += std::max(0.0, std::min(1.0, struct_complexity));
        
        // 適応性計算（安定性と複雑性のバランス）
        double optimal_stability = 0.6;
        double optimal_complexity = 0.7;
        double stability_dev = std::abs(struct_stability - optimal_stability);
        double complexity_dev = std::abs(struct_complexity - optimal_complexity);
        double struct_adaptability = 1.0 - (stability_dev + complexity_dev) / 2.0;
        
        total_adaptability += std::max(0.0, struct_adaptability);
    }
    
    stability = total_stability / count;
    complexity = total_complexity / count;
    adaptability = total_adaptability / count;
}

void SSDUniversalEngine::analyze_pressures(
    const SSDUniversalMeaningPressure* pressures, int32_t count,
    const SSDEvaluationContext* context, const DomainCoefficients& coeff,
    double& magnitude, double& coherence, double& sustainability)
{
    if (count == 0) {
        magnitude = coherence = sustainability = 0.0;
        return;
    }
    
    double total_magnitude = 0.0;
    double total_sustainability = 0.0;
    std::vector<std::vector<double>> direction_vectors;
    
    for (int32_t i = 0; i < count; i++) {
        const auto& p = pressures[i];
        
        // 強度計算
        double press_magnitude = p.magnitude;
        
        // 周波数による補正
        double frequency_factor = 1.0 + std::log(1.0 + p.frequency) * 0.1;
        press_magnitude *= frequency_factor;
        
        // 持続時間による補正
        double duration_factor = std::min(2.0, 1.0 + p.duration / 3600.0);
        press_magnitude *= duration_factor;
        
        // ドメイン補正
        press_magnitude *= coeff.pressure_weight;
        
        total_magnitude += std::max(0.0, std::min(1.0, press_magnitude));
        
        // 方向ベクトル収集
        if (p.direction_dims > 0) {
            std::vector<double> dir(p.direction_dims);
            for (int j = 0; j < p.direction_dims && j < 8; j++) {
                dir[j] = p.direction_vector[j];
            }
            direction_vectors.push_back(dir);
        }
        
        // 持続可能性計算
        double sustainability_val = 0.5; // デフォルト
        switch (p.decay_function) {
            case 0: sustainability_val = 1.0; break; // constant
            case 1: sustainability_val = 0.3; break; // exponential
            case 2: sustainability_val = 0.6; break; // linear
            case 3: sustainability_val = 0.8; break; // logarithmic
        }
        
        total_sustainability += std::max(0.0, std::min(1.0, sustainability_val));
    }
    
    magnitude = total_magnitude / count;
    sustainability = total_sustainability / count;
    
    // 一貫性計算（方向ベクトルの類似度）
    coherence = 1.0; // デフォルト
    if (direction_vectors.size() > 1) {
        double total_similarity = 0.0;
        int pairs = 0;
        
        for (size_t i = 0; i < direction_vectors.size(); i++) {
            for (size_t j = i + 1; j < direction_vectors.size(); j++) {
                const auto& v1 = direction_vectors[i];
                const auto& v2 = direction_vectors[j];
                
                if (v1.size() == v2.size() && !v1.empty()) {
                    // コサイン類似度計算
                    double dot_product = 0.0, norm1 = 0.0, norm2 = 0.0;
                    for (size_t k = 0; k < v1.size(); k++) {
                        dot_product += v1[k] * v2[k];
                        norm1 += v1[k] * v1[k];
                        norm2 += v2[k] * v2[k];
                    }
                    
                    if (norm1 > 0 && norm2 > 0) {
                        double similarity = dot_product / (std::sqrt(norm1) * std::sqrt(norm2));
                        total_similarity += similarity;
                        pairs++;
                    }
                }
            }
        }
        
        if (pairs > 0) {
            coherence = (total_similarity / pairs + 1.0) / 2.0; // -1..1 を 0..1 に正規化
        }
    }
}

void SSDUniversalEngine::analyze_alignment(
    const SSDUniversalStructure* structures, int32_t structure_count,
    const SSDUniversalMeaningPressure* pressures, int32_t pressure_count,
    const SSDEvaluationContext* context, const DomainCoefficients& coeff,
    double& strength, double& efficiency, double& durability)
{
    if (structure_count == 0 || pressure_count == 0) {
        strength = efficiency = durability = 0.0;
        return;
    }
    
    double total_strength = 0.0;
    double total_efficiency = 0.0;
    double total_durability = 0.0;
    int combinations = 0;
    
    for (int32_t i = 0; i < structure_count; i++) {
        for (int32_t j = 0; j < pressure_count; j++) {
            const auto& s = structures[i];
            const auto& p = pressures[j];
            
            // 強度：構造安定性と意味圧のマッチング
            double stability_match = 1.0 - std::abs(s.stability_index - p.magnitude);
            double complexity_factor = 1.0 - s.complexity_level * 0.3;
            double align_strength = stability_match * complexity_factor * coeff.alignment_weight;
            total_strength += std::max(0.0, std::min(1.0, align_strength));
            
            // 効率：複雑性が低いほど効率的
            double base_efficiency = 1.0 - s.complexity_level * 0.5;
            double pressure_factor = 1.0 - p.magnitude * 0.2;
            double align_efficiency = base_efficiency * pressure_factor;
            total_efficiency += std::max(0.0, std::min(1.0, align_efficiency));
            
            // 持久性：構造安定性と意味圧持続性
            double structure_durability = s.stability_index;
            double pressure_persistence = (p.decay_function == 0) ? 1.0 : 0.5;
            double align_durability = structure_durability * pressure_persistence;
            total_durability += std::max(0.0, std::min(1.0, align_durability));
            
            combinations++;
        }
    }
    
    if (combinations > 0) {
        strength = total_strength / combinations;
        efficiency = total_efficiency / combinations;
        durability = total_durability / combinations;
    } else {
        strength = efficiency = durability = 0.0;
    }
}

void SSDUniversalEngine::analyze_jump_potential(
    const SSDUniversalStructure* structures, int32_t structure_count,
    const SSDUniversalMeaningPressure* pressures, int32_t pressure_count,
    const SSDEvaluationContext* context, const DomainCoefficients& coeff,
    double& probability, double* direction, int32_t& direction_dims, double& impact)
{
    probability = 0.0;
    impact = 0.0;
    direction_dims = 3; // デフォルト3次元
    for (int i = 0; i < direction_dims; i++) {
        direction[i] = 0.0;
    }
    
    if (structure_count == 0 || pressure_count == 0) return;
    
    
    // 平均κ（構造安定性）を簡易推定
    double kappa_bar = 0.0;
    for (int32_t i = 0; i < structure_count; i++) { kappa_bar += structures[i].stability_index; }
    if (structure_count > 0) kappa_bar /= structure_count;
std::vector<double> jump_probabilities;
    std::vector<std::vector<double>> jump_directions;
    std::vector<double> jump_impacts;
    std::vector<double> weights;
    
    for (int32_t i = 0; i < structure_count; i++) {
        const auto& s = structures[i];
        double structure_limit = s.stability_index * 0.8; // 構造限界
        
        for (int32_t j = 0; j < pressure_count; j++) {
            const auto& p = pressures[j];
            double pressure_intensity = p.magnitude;
            
            // 跳躍確率（SSD形：σ((E−Θ)/γ)）
            double prob = 0.0;
            double P = std::max(0.0, std::min(1.0, pressure_intensity));
            double J = std::max(0.0, std::min(1.0, kappa_bar * P));
            double E = std::max(0.0, P - J);
            double theta = 0.3 + 0.6 * kappa_bar; // κが高いほど閾値が上がる
            double beta = 0.0; // 将来: context->theoria_beta を導入
            double x = (E - theta) * (1.0 - beta);
            prob = 1.0 / (1.0 + std::exp(-4.0 * x));
            prob = std::max(0.0, std::min(1.0, prob));
            jump_probabilities.push_back(prob);
            weights.push_back(prob);
            
            // 跳躍方向
            std::vector<double> dir(direction_dims, 0.0);
            for (int k = 0; k < std::min(p.direction_dims, direction_dims) && k < 8; k++) {
                dir[k] = p.direction_vector[k];
            }
            jump_directions.push_back(dir);
            
            // 跳躍インパクト
            double impact_val = s.complexity_level * p.magnitude * coeff.jump_weight;
            jump_impacts.push_back(impact_val);
        }
    }
    
    // 平均確率計算
    if (!jump_probabilities.empty()) {
        double sum_prob = 0.0;
        for (double p : jump_probabilities) {
            sum_prob += p;
        }
        probability = sum_prob / jump_probabilities.size();
    }
    
    // 重み付き平均方向計算
    if (!jump_directions.empty() && !weights.empty()) {
        double weight_sum = 0.0;
        for (double w : weights) {
            weight_sum += w;
        }
        
        if (weight_sum > 0) {
            for (int d = 0; d < direction_dims; d++) {
                double weighted_sum = 0.0;
                for (size_t i = 0; i < jump_directions.size(); i++) {
                    if (d < (int)jump_directions[i].size()) {
                        weighted_sum += jump_directions[i][d] * weights[i];
                    }
                }
                direction[d] = weighted_sum / weight_sum;
            }
        }
    }
    
    // 平均インパクト計算
    if (!jump_impacts.empty()) {
        double sum_impact = 0.0;
        for (double imp : jump_impacts) {
            sum_impact += imp;
        }
        impact = sum_impact / jump_impacts.size();
    }
}

void SSDUniversalEngine::integrate_analyses(
    double struct_stability, double struct_complexity, double struct_adaptability,
    double press_magnitude, double press_coherence, double press_sustainability,
    double align_strength, double align_efficiency, double align_durability,
    double jump_probability, double jump_impact,
    double& system_health, double& evolution_potential, double& stability_resilience)
{
    // システム健全性：全体的な安定性と機能性
    system_health = (
        struct_stability * 0.3 +
        align_strength * 0.3 +
        align_efficiency * 0.2 +
        (1.0 - jump_probability) * 0.2
    );
    
    // 進化ポテンシャル：成長と変化の可能性
    evolution_potential = (
        struct_adaptability * 0.4 +
        press_sustainability * 0.3 +
        jump_probability * 0.3
    );
    
    // 安定性回復力：外乱からの回復能力
    stability_resilience = (
        struct_stability * 0.4 +
        align_durability * 0.3 +
        press_coherence * 0.3
    );
    
    // 範囲制限
    system_health = std::max(0.0, std::min(1.0, system_health));
    evolution_potential = std::max(0.0, std::min(1.0, evolution_potential));
    stability_resilience = std::max(0.0, std::min(1.0, stability_resilience));
}

void SSDUniversalEngine::generate_warnings_and_recommendations(
    const SSDUniversalEvaluationResult& result,
    uint32_t& warnings, uint32_t& recommendations)
{
    warnings = 0;
    recommendations = 0;
    
    // 警告生成
    if (result.system_health < 0.3) {
        warnings |= SSD_WARNING_LOW_HEALTH;
    }
    if (result.jump_probability > 0.7) {
        warnings |= SSD_WARNING_HIGH_JUMP_RISK;
    }
    if (result.stability_resilience < 0.4) {
        warnings |= SSD_WARNING_LOW_RESILIENCE;
    }
    if (result.calculation_confidence < 0.5) {
        warnings |= SSD_WARNING_LOW_CONFIDENCE;
    }
    if (result.structure_complexity > 0.8) {
        warnings |= SSD_WARNING_HIGH_COMPLEXITY;
    }
    
    // 推奨生成
    if (result.system_health < 0.6) {
        recommendations |= SSD_RECOMMEND_STABILIZE;
    }
    if (result.pressure_magnitude > 0.8) {
        recommendations |= SSD_RECOMMEND_REDUCE_PRESSURE;
    }
    if (result.evolution_potential < 0.3) {
        recommendations |= SSD_RECOMMEND_INCREASE_CHANGE;
    }
    if (result.stability_resilience < 0.5) {
        recommendations |= SSD_RECOMMEND_ADD_REDUNDANCY;
    }
    if (result.calculation_confidence < 0.7) {
        recommendations |= SSD_RECOMMEND_MONITOR_CLOSELY;
    }
}

double SSDUniversalEngine::calculate_confidence(
    const SSDUniversalStructure* structures, int32_t structure_count,
    const SSDUniversalMeaningPressure* pressures, int32_t pressure_count,
    const SSDEvaluationContext* context)
{
    // データ品質
    double data_quality = std::min(structure_count / 5.0, 1.0);
    double pressure_quality = std::min(pressure_count / 3.0, 1.0);
    
    // 精度レベル
    double precision_factors[] = {0.5, 0.7, 0.9, 1.0};
    double precision_factor = precision_factors[std::min(config.precision_level, 3)];
    
    // 測定精度
    double measurement_factor = context->measurement_precision;
    
    return (data_quality + pressure_quality + precision_factor + measurement_factor) / 4.0;
}

size_t SSDUniversalEngine::calculate_hash(
    const SSDUniversalStructure* structures, int32_t structure_count,
    const SSDUniversalMeaningPressure* pressures, int32_t pressure_count,
    const SSDEvaluationContext* context)
{
    // 簡易ハッシュ計算
    size_t hash = 0;
    
    for (int32_t i = 0; i < structure_count; i++) {
        hash ^= std::hash<std::string>{}(structures[i].structure_id) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    
    for (int32_t i = 0; i < pressure_count; i++) {
        hash ^= std::hash<std::string>{}(pressures[i].pressure_id) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    // コンテキスト要素をハッシュに反映
    hash ^= std::hash<int>{}(static_cast<int>(context->domain)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= std::hash<int>{}(context->scale_level) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= std::hash<long long>{}(std::llround(context->time_scale * 1e6)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= std::hash<long long>{}(std::llround(context->measurement_precision * 1000)) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    hash ^= std::hash<std::string>{}(context->context_id) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    
    return hash;
}

void SSDUniversalEngine::update_statistics(double computation_time, double confidence) {
    std::lock_guard<std::mutex> lock(stats_mutex);
    total_evaluations++;
    total_computation_time += computation_time;
    
    recent_accuracy_scores.push_back(confidence);
    if (recent_accuracy_scores.size() > 100) {
        recent_accuracy_scores.erase(recent_accuracy_scores.begin());
    }
}

SSDReturnCode SSDUniversalEngine::calculate_inertia_unified(
    SSDStructureLayer layer, SSDInertiaType type,
    const SSDInertiaComponent* components, int32_t count,
    const SSDEvaluationContext* context,
    double* out_inertia, double* out_confidence)
{
    if (count == 0 || !components) {
        *out_inertia = 0.0;
        *out_confidence = 1.0;
        return SSD_SUCCESS;
    }
    
    // 慣性タイプ特性
    struct InertiaTypeCharacteristics {
        double formation_speed;
        double stability_base;
        double context_dependency;
        double decay_rate;
    };
    
    InertiaTypeCharacteristics type_chars[] = {
        {0.6, 0.7, 0.8, 0.02},  // ACTION
        {0.3, 0.9, 0.6, 0.005}, // ROUTINE
        {0.8, 0.6, 0.9, 0.03},  // SOCIAL
        {0.7, 0.8, 0.7, 0.01},  // SPATIAL
        {0.5, 0.8, 0.5, 0.008}, // TEMPORAL
        {0.4, 0.9, 0.4, 0.001}, // COGNITIVE
        {0.9, 0.4, 1.0, 0.05},  // EMOTIONAL
        {0.7, 0.5, 0.8, 0.025}  // CREATIVE
    };
    
    InertiaTypeCharacteristics chars = type_chars[type];
    
    // 基礎慣性計算
    double total_weighted_strength = 0.0;
    double total_weight = 0.0;
    double current_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    for (int32_t i = 0; i < count; i++) {
        const auto& comp = components[i];
        
        // 基礎強度の重み付け
        double strength_weight = (
            comp.usage_frequency * 0.3 +
            comp.success_rate * 0.4 +
            comp.temporal_stability * 0.3
        );
        
        // 強化による増幅
        double reinforcement_bonus = std::min(0.3, comp.reinforcement_count * 0.005);
        
        // タイプ特性による調整
        double type_adjusted_strength = comp.base_strength * chars.stability_base * (1.0 + reinforcement_bonus);
        
        // 時間減衰
        double time_since_activation = current_time - comp.last_activation;
        double decay_factor = std::exp(-chars.decay_rate * time_since_activation / 3600.0);
        type_adjusted_strength *= decay_factor;
        
        double weighted_strength = type_adjusted_strength * strength_weight;
        total_weighted_strength += weighted_strength;
        total_weight += strength_weight;
    }
    
    double base_inertia = (total_weight > 0) ? (total_weighted_strength / total_weight) : 0.0;
    
    // 構造層重み適用
    double layer_weight = config.layer_weights[layer];
    *out_inertia = std::max(0.0, std::min(1.0, base_inertia * layer_weight));
    
    // 信頼度計算
    double data_quality = std::min(count / 5.0, 1.0);
    double avg_reinforcement = 0.0;
    for (int32_t i = 0; i < count; i++) {
        avg_reinforcement += components[i].reinforcement_count;
    }
    avg_reinforcement /= count;
    double reinforcement_quality = std::min(avg_reinforcement / 50.0, 1.0);
    
    *out_confidence = (data_quality + reinforcement_quality) / 2.0;
    
    return SSD_SUCCESS;
}

/* ========================================
 * C API実装
 * ======================================== */

extern "C" {

SSD_UNIVERSAL_API SSDUniversalEngine* ssd_universal_create(const SSDEngineConfig* config) {
    try {
        return new SSDUniversalEngine(config);
    } catch (...) {
        return nullptr;
    }
}

SSD_UNIVERSAL_API void ssd_universal_destroy(SSDUniversalEngine* engine) {
    delete engine;
}

SSD_UNIVERSAL_API SSDReturnCode ssd_universal_get_config(SSDUniversalEngine* engine, SSDEngineConfig* out_config) {
    if (!engine || !out_config) return SSD_ERROR_INVALID_INPUT;
    *out_config = engine->config;
    return SSD_SUCCESS;
}

SSD_UNIVERSAL_API SSDReturnCode ssd_universal_set_config(SSDUniversalEngine* engine, const SSDEngineConfig* config) {
    if (!engine || !config) return SSD_ERROR_INVALID_INPUT;
    engine->config = *config;
    return SSD_SUCCESS;
}

SSD_UNIVERSAL_API SSDReturnCode ssd_universal_get_stats(SSDUniversalEngine* engine, SSDEngineStats* out_stats) {
    if (!engine || !out_stats) return SSD_ERROR_INVALID_INPUT;
    
    memset(out_stats, 0, sizeof(SSDEngineStats));
    strncpy(out_stats->engine_id, engine->engine_id.c_str(), sizeof(out_stats->engine_id) - 1);
    strncpy(out_stats->version, engine->version.c_str(), sizeof(out_stats->version) - 1);
    
    out_stats->total_evaluations = engine->total_evaluations;
    out_stats->average_computation_time = (engine->total_evaluations > 0) ? 
        (engine->total_computation_time / engine->total_evaluations) : 0.0;
    out_stats->cache_hit_rate = (engine->total_evaluations > 0) ? 
        (double(engine->cache_hits) / engine->total_evaluations) : 0.0;
    
    if (!engine->recent_accuracy_scores.empty()) {
        double sum = 0.0;
        for (double score : engine->recent_accuracy_scores) sum += score;
        out_stats->accuracy_score = sum / engine->recent_accuracy_scores.size();
    }
    
    auto current_time = std::chrono::steady_clock::now();
    out_stats->uptime_seconds = std::chrono::duration_cast<std::chrono::seconds>(current_time - engine->start_time).count();
    out_stats->cache_size = static_cast<int32_t>(engine->cache.size());
    out_stats->max_cache_size = 1000;
    
    return SSD_SUCCESS;
}

SSD_UNIVERSAL_API SSDReturnCode ssd_universal_reset(SSDUniversalEngine* engine) {
    if (!engine) return SSD_ERROR_INVALID_INPUT;
    
    engine->cache.clear();
    engine->total_evaluations = 0;
    engine->total_computation_time = 0.0;
    engine->cache_hits = 0;
    engine->recent_accuracy_scores.clear();
    engine->start_time = std::chrono::steady_clock::now();
    
    return SSD_SUCCESS;
}

SSD_UNIVERSAL_API SSDReturnCode ssd_evaluate_universal_system(
    SSDUniversalEngine* engine,
    const SSDUniversalStructure* structures,
    int32_t structure_count,
    const SSDUniversalMeaningPressure* meaning_pressures,
    int32_t pressure_count,
    const SSDEvaluationContext* context,
    SSDUniversalEvaluationResult* out_result)
{
    if (!engine) return SSD_ERROR_INVALID_INPUT;
    return engine->evaluate_system(structures, structure_count, meaning_pressures, pressure_count, context, out_result);
}

SSD_UNIVERSAL_API const char* ssd_get_version_string(void) {
    return "SSD Universal Engine v1.0.0";
}

SSD_UNIVERSAL_API void ssd_get_version_info(int32_t* major, int32_t* minor, int32_t* patch) {
    if (major) *major = 1;
    if (minor) *minor = 0;
    if (patch) *patch = 0;
}

SSD_UNIVERSAL_API const char* ssd_get_last_error_message(SSDUniversalEngine* engine) {
    return engine ? engine->last_error.c_str() : "Invalid engine handle";
}

SSD_UNIVERSAL_API double ssd_get_memory_usage_mb(SSDUniversalEngine* engine) {
    if (!engine) return 0.0;
    return sizeof(SSDUniversalEngine) / (1024.0 * 1024.0) + 
           engine->cache.size() * sizeof(CacheEntry) / (1024.0 * 1024.0);
}

/* 高レベル便利関数の実装 */

SSD_UNIVERSAL_API SSDReturnCode ssd_evaluate_npc_action(
    SSDUniversalEngine* engine,
    const char* action_id,
    const char* entity_id,
    double basal_drives[8],
    int32_t basal_count,
    double routine_strengths[16],
    int32_t routine_count,
    double episodic_influences[8],
    int32_t episodic_count,
    double environmental_factors[8],
    int32_t env_count,
    double* out_action_inertia,
    double* out_confidence,
    char* out_reasoning,
    int32_t reasoning_size)
{
    if (!engine || !action_id || !out_action_inertia || !out_confidence) {
        return SSD_ERROR_INVALID_INPUT;
    }
    
    try {
        // 慣性コンポーネント構築
        std::vector<SSDInertiaComponent> components;
        
        // 基層衝動を慣性コンポーネントに変換
        for (int32_t i = 0; i < basal_count && i < 8; i++) {
            SSDInertiaComponent comp;
            memset(&comp, 0, sizeof(comp));
            snprintf(comp.component_id, sizeof(comp.component_id), "basal_drive_%d", i);
            comp.base_strength = std::max(0.0, std::min(1.0, basal_drives[i]));
            comp.usage_frequency = 0.8;
            comp.success_rate = 0.9;
            comp.last_activation = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            comp.temporal_stability = 0.95;
            comp.reinforcement_count = 1000;
            comp.decay_resistance = 0.98;
            comp.binding_count = 0;
            components.push_back(comp);
        }
        
        // 習慣強度を慣性コンポーネントに変換
        for (int32_t i = 0; i < routine_count && i < 16; i++) {
            SSDInertiaComponent comp;
            memset(&comp, 0, sizeof(comp));
            snprintf(comp.component_id, sizeof(comp.component_id), "routine_%d", i);
            comp.base_strength = std::max(0.0, std::min(1.0, routine_strengths[i]));
            comp.usage_frequency = 0.6;
            comp.success_rate = 0.8;
            comp.last_activation = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() - 3600;
            comp.temporal_stability = 0.8;
            comp.reinforcement_count = static_cast<int32_t>(routine_strengths[i] * 50);
            comp.decay_resistance = 0.7;
            comp.binding_count = 0;
            components.push_back(comp);
        }
        
        // エピソード記憶を慣性コンポーネントに変換
        for (int32_t i = 0; i < episodic_count && i < 8; i++) {
            SSDInertiaComponent comp;
            memset(&comp, 0, sizeof(comp));
            snprintf(comp.component_id, sizeof(comp.component_id), "episodic_%d", i);
            comp.base_strength = std::abs(episodic_influences[i]);
            comp.usage_frequency = 0.3;
            comp.success_rate = (episodic_influences[i] > 0) ? 0.9 : 0.2;
            comp.last_activation = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count() - 7200;
            comp.temporal_stability = std::abs(episodic_influences[i]);
            comp.reinforcement_count = static_cast<int32_t>(std::abs(episodic_influences[i]) * 10);
            comp.decay_resistance = std::abs(episodic_influences[i]) * 0.8;
            comp.binding_count = 0;
            components.push_back(comp);
        }
        // 環境要因の平均で慣性強度を微調整（0.9〜1.1倍）
        if (env_count > 0) {
            double env_sum = 0.0; int n = std::min(env_count, 8);
            for (int i = 0; i < n; ++i) env_sum += environmental_factors[i];
            double env = env_sum / n; // 想定範囲: 0..1
            double factor = std::max(0.9, std::min(1.1, 0.9 + 0.2 * env));
            for (auto& c : components) {
                c.base_strength = std::max(0.0, std::min(1.0, c.base_strength * factor));
            }
        }

        
        // 評価コンテキスト構築
        SSDEvaluationContext context;
        memset(&context, 0, sizeof(context));
        strncpy(context.context_id, action_id, sizeof(context.context_id) - 1);
        context.context_id[sizeof(context.context_id) - 1] = '\0';
        context.domain = SSD_DOMAIN_AI;
        context.scale_level = SSD_SCALE_ORGANISM;
        context.time_scale = 1.0;
        context.space_scale = 1.0;
        context.observer_position[0] = context.observer_position[1] = context.observer_position[2] = 0.0;
        context.measurement_precision = 0.8;
        context.env_factor_count = std::min(env_count, 8);
        for (int32_t i = 0; i < context.env_factor_count; i++) {
            context.environmental_factors[i] = environmental_factors[i];
        }
        
        // 包括的慣性計算
        double layer_breakdown[4];
        char explanation[512];
        
        SSDReturnCode result = ssd_calculate_comprehensive_inertia(
            engine,
            nullptr, 0,  // physical
            components.data(), basal_count,  // basal
            components.data() + basal_count, routine_count,  // core
            components.data() + basal_count + routine_count, episodic_count,  // upper
            &context,
            out_action_inertia,
            layer_breakdown,
            explanation, sizeof(explanation)
        );
        
        if (result == SSD_SUCCESS || result == SSD_WARNING_LOW_CONFIDENCE) {
            // 信頼度計算
            *out_confidence = engine->calculate_confidence(nullptr, 0, nullptr, 0, &context);
            
            // 推論理由生成
            if (out_reasoning && reasoning_size > 0) {
                snprintf(out_reasoning, reasoning_size,
                    "Action inertia %.3f = Basal(%.3f) + Routine(%.3f) + Memory(%.3f). %s",
                    *out_action_inertia,
                    layer_breakdown[1], layer_breakdown[2], layer_breakdown[3],
                    explanation);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        engine->last_error = std::string("NPC evaluation failed: ") + e.what();
        return SSD_ERROR_CALCULATION_FAILED;
    }
}

SSD_UNIVERSAL_API SSDReturnCode ssd_calculate_comprehensive_inertia(
    SSDUniversalEngine* engine,
    const SSDInertiaComponent* physical_components,
    int32_t physical_count,
    const SSDInertiaComponent* basal_components,
    int32_t basal_count,
    const SSDInertiaComponent* core_components,
    int32_t core_count,
    const SSDInertiaComponent* upper_components,
    int32_t upper_count,
    const SSDEvaluationContext* context,
    double* out_total_inertia,
    double* out_layer_breakdown,
    char* out_explanation,
    int32_t explanation_size)
{
    if (!engine || !context || !out_total_inertia) {
        return SSD_ERROR_INVALID_INPUT;
    }
    
    try {
        double layer_inertias[4] = {0.0, 0.0, 0.0, 0.0};
        double layer_confidences[4] = {0.0, 0.0, 0.0, 0.0};
        
        // 各層の慣性計算
        if (physical_components && physical_count > 0) {
            engine->calculate_inertia_unified(SSD_LAYER_PHYSICAL, SSD_INERTIA_ACTION,
                physical_components, physical_count, context,
                &layer_inertias[0], &layer_confidences[0]);
        }
        
        if (basal_components && basal_count > 0) {
            engine->calculate_inertia_unified(SSD_LAYER_BASAL, SSD_INERTIA_ACTION,
                basal_components, basal_count, context,
                &layer_inertias[1], &layer_confidences[1]);
        }
        
        if (core_components && core_count > 0) {
            engine->calculate_inertia_unified(SSD_LAYER_CORE, SSD_INERTIA_ROUTINE,
                core_components, core_count, context,
                &layer_inertias[2], &layer_confidences[2]);
        }
        
        if (upper_components && upper_count > 0) {
            engine->calculate_inertia_unified(SSD_LAYER_UPPER, SSD_INERTIA_SOCIAL,
                upper_components, upper_count, context,
                &layer_inertias[3], &layer_confidences[3]);
        }
        
        // 構造層重みによる統合
        double total_weighted_inertia = 0.0;
        double total_weight = 0.0;
        
        for (int i = 0; i < 4; i++) {
            double weight = engine->config.layer_weights[i];
            total_weighted_inertia += layer_inertias[i] * weight;
            total_weight += weight;
            
            if (out_layer_breakdown) {
                out_layer_breakdown[i] = layer_inertias[i];
            }
        }
        
        *out_total_inertia = (total_weight > 0) ? (total_weighted_inertia / total_weight) : 0.0;
        
        // 説明文生成
        if (out_explanation && explanation_size > 0) {
            snprintf(out_explanation, explanation_size,
                "Comprehensive inertia %.3f = Physical(%.3f)*%.1f + Basal(%.3f)*%.1f + Core(%.3f)*%.1f + Upper(%.3f)*%.1f",
                *out_total_inertia,
                layer_inertias[0], engine->config.layer_weights[0],
                layer_inertias[1], engine->config.layer_weights[1],
                layer_inertias[2], engine->config.layer_weights[2],
                layer_inertias[3], engine->config.layer_weights[3]);
        }
        
        return SSD_SUCCESS;
        
    } catch (const std::exception& e) {
        engine->last_error = std::string("Comprehensive inertia calculation failed: ") + e.what();
        return SSD_ERROR_CALCULATION_FAILED;
    }
}

} /* extern "C" */