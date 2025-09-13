#pragma once
#include "../core/ssd_core.h"
#include "../core/neuro_core.h"

#ifdef _WIN32
  #ifdef SSD_API_EXPORTS
    #define UNIFIED_API __declspec(dllexport)
  #else
    #define UNIFIED_API __declspec(dllimport)
  #endif
#else
  #define UNIFIED_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

// 前方宣言
typedef struct NeuroSSDSystem NeuroSSDSystem;

// === 基本ライフサイクル ===
UNIFIED_API NeuroSSDSystem* neurossd_create(int32_t N, uint64_t seed);
UNIFIED_API void neurossd_destroy(NeuroSSDSystem* sys);

// === メイン更新 ===
UNIFIED_API void neurossd_tick(NeuroSSDSystem* sys, double meaning_pressure, float dt_sec, SSDTelemetry* out);

// === イベント処理 ===
UNIFIED_API void neurossd_apply_event(NeuroSSDSystem* sys, const char* event_id);

// === 状態取得 ===
UNIFIED_API void neurossd_get_neuro_state(NeuroSSDSystem* sys, NeuroState* out);
UNIFIED_API void neurossd_get_ssd_params(NeuroSSDSystem* sys, SSDParams* out);

// === 設定 ===
UNIFIED_API void neurossd_set_neuro_baseline(NeuroSSDSystem* sys, const NeuroState* baseline);
UNIFIED_API void neurossd_get_neuro_baseline(NeuroSSDSystem* sys, NeuroState* out);

// === デバッグ・監視 ===
UNIFIED_API int32_t neurossd_get_current_node(NeuroSSDSystem* sys);
UNIFIED_API double neurossd_get_heat_level(NeuroSSDSystem* sys);

#ifdef __cplusplus
}
#endif