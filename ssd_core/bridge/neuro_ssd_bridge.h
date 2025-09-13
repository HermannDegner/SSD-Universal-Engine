#pragma once
#include "../core/neuro_core.h"
#include "../core/ssd_core.h"

// 神経状態からSSDパラメータへの写像
void MapNeuroToSSD(const NeuroState& neuro_state, SSDParams& ssd_params);

// 統合システム
struct NeuroSSDSystem {
  NeuroCore neuro;
  SSDHandle* ssd_handle;
  
  NeuroSSDSystem(int32_t N, uint64_t seed = 0);
  ~NeuroSSDSystem();
  
  void Tick(double meaning_pressure, float dt_sec, SSDTelemetry* telemetry = nullptr);
  void ApplyEvent(std::string_view event_id);
  
  const NeuroState& GetNeuroState() const { return neuro.Get(); }
  void GetSSDParams(SSDParams* out) const { ssd_get_params(ssd_handle, out); }
};