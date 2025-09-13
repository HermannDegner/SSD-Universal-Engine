#include "neuro_ssd_bridge.h"
#include <algorithm>

static inline double dev(float u01) { return 2.0 * double(u01) - 1.0; }
static inline double clip01(double v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }
static inline double clipPos(double v) { return v < 1e-6 ? 1e-6 : v; }

void MapNeuroToSSD(const NeuroState& n, SSDParams& prm) {
  double DA = dev(n.DA), S5 = dev(n.S5), NE = dev(n.NE);
  double AD = dev(n.AD), END = dev(n.END), OXT = dev(n.OXT), CORT = dev(n.CORT);
  
  // 小さな安全係数で写像
  const double k1 = 0.20, k2 = 0.15, k3 = 0.10;
  const double a1 = 0.25, a2 = 0.20, a3 = 0.25;
  const double b1 = 0.20, b2 = 0.15, b3 = 0.15;
  const double c1 = 0.10, c2 = 0.10, d1 = 0.10;
  const double e1 = 0.15, e2 = 0.10, f1 = 0.10, f2 = 0.10;
  const double s1 = 0.05, s2 = 0.05;

  prm.T0 = clip01(prm.T0 + k1*DA - k2*NE + k3*AD);
  prm.Theta0 = clip01(prm.Theta0 + a1*S5 + a2*OXT - a3*CORT);
  prm.h0 = clip01(prm.h0 + b1*DA + b2*AD - b3*S5);
  prm.eta = clipPos(prm.eta + c1*DA - c2*CORT);
  prm.lam = clipPos(prm.lam + d1*S5);
  prm.alpha = clipPos(prm.alpha + e1*NE - e2*END);
  prm.beta_E = clipPos(prm.beta_E + f1*S5 + f2*END);
  prm.sigma = clip01(prm.sigma + s1*DA - s2*S5);
}

NeuroSSDSystem::NeuroSSDSystem(int32_t N, uint64_t seed) {
  SSDParams default_params{}; // デフォルト初期化
  ssd_handle = ssd_create(N, &default_params, seed ? seed : 123456789ULL);
}

NeuroSSDSystem::~NeuroSSDSystem() {
  if (ssd_handle) ssd_destroy(ssd_handle);
}

void NeuroSSDSystem::Tick(double meaning_pressure, float dt_sec, SSDTelemetry* telemetry) {
  // 1. 神経系更新
  neuro.Tick(dt_sec);
  
  // 2. 神経→SSD写像
  SSDParams current_params;
  ssd_get_params(ssd_handle, &current_params);
  MapNeuroToSSD(neuro.Get(), current_params);
  ssd_set_params(ssd_handle, &current_params);
  
  // 3. SSD更新
  ssd_step(ssd_handle, meaning_pressure, double(dt_sec), telemetry);
}

void NeuroSSDSystem::ApplyEvent(std::string_view event_id) {
  neuro.ApplyEvent(event_id);
}