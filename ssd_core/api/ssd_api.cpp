#include "ssd_api.h"
#include "../bridge/neuro_ssd_bridge.h"

// C APIラッパー実装

extern "C" NeuroSSDSystem* neurossd_create(int32_t N, uint64_t seed) {
    try {
        return new NeuroSSDSystem(N, seed);
    } catch (...) {
        return nullptr;
    }
}

extern "C" void neurossd_destroy(NeuroSSDSystem* sys) {
    delete sys;
}

extern "C" void neurossd_tick(NeuroSSDSystem* sys, double meaning_pressure, float dt_sec, SSDTelemetry* out) {
    if (!sys) return;
    sys->Tick(meaning_pressure, dt_sec, out);
}

extern "C" void neurossd_apply_event(NeuroSSDSystem* sys, const char* event_id) {
    if (!sys || !event_id) return;
    sys->ApplyEvent(std::string_view(event_id));
}

extern "C" void neurossd_get_neuro_state(NeuroSSDSystem* sys, NeuroState* out) {
    if (!sys || !out) return;
    *out = sys->GetNeuroState();
}

// 追加のユーティリティ関数

extern "C" void neurossd_get_ssd_params(NeuroSSDSystem* sys, SSDParams* out) {
    if (!sys || !out) return;
    sys->GetSSDParams(out);
}

extern "C" void neurossd_set_neuro_baseline(NeuroSSDSystem* sys, const NeuroState* baseline) {
    if (!sys || !baseline) return;
    sys->neuro.baseline = *baseline;
}

extern "C" void neurossd_get_neuro_baseline(NeuroSSDSystem* sys, NeuroState* out) {
    if (!sys || !out) return;
    *out = sys->neuro.baseline;
}

// デバッグ・監視用
extern "C" int32_t neurossd_get_current_node(NeuroSSDSystem* sys) {
    if (!sys) return -1;
    
    SSDTelemetry telem;
    ssd_step(sys->ssd_handle, 0.0, 0.0, &telem);  // ダミー呼び出し
    return telem.current;
}

extern "C" double neurossd_get_heat_level(NeuroSSDSystem* sys) {
    if (!sys) return 0.0;
    
    SSDTelemetry telem;
    ssd_step(sys->ssd_handle, 0.0, 0.0, &telem);  // ダミー呼び出し
    return telem.E;
}