/*
 * ssd_universal_engine_dll.h
 * 構造主観力学（SSD）汎用評価エンジン DLL インターフェース
 * 
 * あらゆる構造・意味圧・整合・跳躍現象を統一的に評価
 */

#ifndef SSD_UNIVERSAL_ENGINE_DLL_H
#define SSD_UNIVERSAL_ENGINE_DLL_H

#ifdef _WIN32
  #ifdef SSD_UNIVERSAL_DLL_EXPORTS
    #define SSD_UNIVERSAL_API __declspec(dllexport)
  #else
    #define SSD_UNIVERSAL_API __declspec(dllimport)
  #endif
#else
  #define SSD_UNIVERSAL_API __attribute__((visibility("default")))
#endif

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================
 * リターンコード
 * ======================================== */
typedef enum {
    SSD_SUCCESS = 0,
    SSD_ERROR_INVALID_INPUT = -1,
    SSD_ERROR_CALCULATION_FAILED = -2,
    SSD_ERROR_INSUFFICIENT_DATA = -3,
    SSD_ERROR_MEMORY_ALLOCATION = -4,
    SSD_WARNING_LOW_CONFIDENCE = 1
} SSDReturnCode;

/* ========================================
 * 構造層と慣性タイプ
 * ======================================== */
typedef enum {
    SSD_LAYER_PHYSICAL = 0,    /* 物理構造 */
    SSD_LAYER_BASAL = 1,       /* 基層構造 */
    SSD_LAYER_CORE = 2,        /* 中核構造 */
    SSD_LAYER_UPPER = 3        /* 上層構造 */
} SSDStructureLayer;

typedef enum {
    SSD_INERTIA_ACTION = 0,      /* 行動パターン慣性 */
    SSD_INERTIA_ROUTINE = 1,     /* 習慣・ルーチン慣性 */
    SSD_INERTIA_SOCIAL = 2,      /* 社交関係慣性 */
    SSD_INERTIA_SPATIAL = 3,     /* 空間・場所慣性 */
    SSD_INERTIA_TEMPORAL = 4,    /* 時間パターン慣性 */
    SSD_INERTIA_COGNITIVE = 5,   /* 認知・思考慣性 */
    SSD_INERTIA_EMOTIONAL = 6,   /* 感情反応慣性 */
    SSD_INERTIA_CREATIVE = 7     /* 創造活動慣性 */
} SSDInertiaType;

/* ========================================
 * ドメインとスケール
 * ======================================== */
typedef enum {
    SSD_DOMAIN_PHYSICS = 0,
    SSD_DOMAIN_CHEMISTRY = 1,
    SSD_DOMAIN_BIOLOGY = 2,
    SSD_DOMAIN_PSYCHOLOGY = 3,
    SSD_DOMAIN_SOCIOLOGY = 4,
    SSD_DOMAIN_ECONOMICS = 5,
    SSD_DOMAIN_AI = 6,
    SSD_DOMAIN_CUSTOM = 99
} SSDDomain;

typedef enum {
    SSD_SCALE_QUANTUM = 0,
    SSD_SCALE_ATOMIC = 1,
    SSD_SCALE_MOLECULAR = 2,
    SSD_SCALE_CELLULAR = 3,
    SSD_SCALE_ORGANISM = 4,
    SSD_SCALE_GROUP = 5,
    SSD_SCALE_SOCIETY = 6,
    SSD_SCALE_CIVILIZATION = 7
} SSDScale;

/* ========================================
 * データ構造
 * ======================================== */

/* 汎用構造定義 */
typedef struct {
    char structure_id[64];
    char structure_type[32];
    int32_t dimension_count;
    double stability_index;      /* 0-1 */
    double complexity_level;     /* 0-1 */
    double dynamic_properties[16]; /* 最大16個の動的特性 */
    int32_t dynamic_count;       /* 実際の動的特性数 */
    double constraint_matrix[16]; /* 4x4行列（平坦化） */
    int32_t constraint_rows;
    int32_t constraint_cols;
} SSDUniversalStructure;

/* 汎用意味圧定義 */
typedef struct {
    char pressure_id[64];
    char source_type[32];
    double magnitude;            /* 0-1 */
    double direction_vector[8];  /* 最大8次元 */
    int32_t direction_dims;
    double frequency;            /* Hz */
    double duration;             /* 秒 */
    double propagation_speed;
    int32_t decay_function;      /* 0=const, 1=exp, 2=linear, 3=log */
    double interaction_matrix[16]; /* 4x4行列（平坦化） */
    int32_t interaction_rows;
    int32_t interaction_cols;
} SSDUniversalMeaningPressure;

/* 評価文脈 */
typedef struct {
    char context_id[64];
    SSDDomain domain;
    SSDScale scale_level;
    double time_scale;           /* 秒 */
    double space_scale;          /* メートル */
    double observer_position[3]; /* x, y, z */
    double measurement_precision; /* 0-1 */
    double environmental_factors[8]; /* 最大8個の環境要因 */
    int32_t env_factor_count;
} SSDEvaluationContext;

/* 慣性コンポーネント */
typedef struct {
    char component_id[64];
    double base_strength;        /* 0-1 */
    double usage_frequency;      /* 0-1 */
    double success_rate;         /* 0-1 */
    double last_activation;      /* timestamp */
    double temporal_stability;   /* 0-1 */
    int32_t reinforcement_count;
    double decay_resistance;     /* 0-1 */
    double contextual_binding[8]; /* key-value pairs (simplified) */
    int32_t binding_count;
} SSDInertiaComponent;

/* 汎用評価結果 */
typedef struct {
    char evaluation_id[64];
    SSDReturnCode return_code;
    
    /* 構造分析結果 */
    double structure_stability;
    double structure_complexity;
    double structure_adaptability;
    
    /* 意味圧分析結果 */
    double pressure_magnitude;
    double pressure_coherence;
    double pressure_sustainability;
    
    /* 整合分析結果 */
    double alignment_strength;
    double alignment_efficiency;
    double alignment_durability;
    
    /* 跳躍分析結果 */
    double jump_probability;
    double jump_direction[8];    /* 予測方向ベクトル */
    int32_t jump_direction_dims;
    double jump_impact_estimation;
    
    /* 総合指標 */
    double system_health;        /* システム健全性 0-1 */
    double evolution_potential;  /* 進化ポテンシャル 0-1 */
    double stability_resilience; /* 安定性回復力 0-1 */
    
    /* メタ情報 */
    double calculation_confidence; /* 計算信頼度 0-1 */
    double computational_cost;     /* 計算時間（秒） */
    double prediction_horizon;     /* 予測可能期間（秒） */
    
    /* 警告・推奨（フラグとして） */
    uint32_t warning_flags;        /* ビットフラグ */
    uint32_t recommendation_flags; /* ビットフラグ */
    
    /* 詳細説明（JSON文字列への参照） */
    char explanation_json[1024];
} SSDUniversalEvaluationResult;

/* エンジン設定 */
typedef struct {
    int32_t precision_level;     /* 0=low, 1=med, 2=high, 3=ultra */
    int32_t calculation_mode;    /* 0=fast, 1=balanced, 2=accurate */
    int32_t enable_cache;        /* 0=false, 1=true */
    int32_t enable_prediction;   /* 0=false, 1=true */
    int32_t enable_explanation;  /* 0=false, 1=true */
    int32_t max_iterations;
    double convergence_threshold;
    int32_t time_limit_ms;
    int32_t parallel_processing; /* 0=false, 1=true */
    int32_t memory_limit_mb;
    
    /* ドメイン特化係数 */
    double domain_weights[8];    /* 各ドメインの重み */
    double layer_weights[4];     /* 各構造層の重み */
} SSDEngineConfig;

/* エンジン統計情報 */
typedef struct {
    char engine_id[64];
    char version[32];
    uint64_t total_evaluations;
    double average_computation_time;
    double cache_hit_rate;
    double accuracy_score;
    uint64_t uptime_seconds;
    int32_t cache_size;
    int32_t max_cache_size;
    double memory_usage_mb;
} SSDEngineStats;

/* ========================================
 * オペークハンドル
 * ======================================== */
typedef struct SSDUniversalEngine SSDUniversalEngine;

/* ========================================
 * エンジン管理API
 * ======================================== */

/* エンジン作成・破棄 */
SSD_UNIVERSAL_API SSDUniversalEngine* ssd_universal_create(const SSDEngineConfig* config);
SSD_UNIVERSAL_API void ssd_universal_destroy(SSDUniversalEngine* engine);

/* 設定取得・変更 */
SSD_UNIVERSAL_API SSDReturnCode ssd_universal_get_config(SSDUniversalEngine* engine, SSDEngineConfig* out_config);
SSD_UNIVERSAL_API SSDReturnCode ssd_universal_set_config(SSDUniversalEngine* engine, const SSDEngineConfig* config);

/* 統計情報取得 */
SSD_UNIVERSAL_API SSDReturnCode ssd_universal_get_stats(SSDUniversalEngine* engine, SSDEngineStats* out_stats);

/* エンジンリセット */
SSD_UNIVERSAL_API SSDReturnCode ssd_universal_reset(SSDUniversalEngine* engine);

/* ========================================
 * 慣性計算API
 * ======================================== */

/* 単一慣性計算 */
SSD_UNIVERSAL_API SSDReturnCode ssd_calculate_inertia(
    SSDUniversalEngine* engine,
    SSDStructureLayer layer,
    SSDInertiaType inertia_type,
    const SSDInertiaComponent* components,
    int32_t component_count,
    const SSDEvaluationContext* context,
    double* out_total_inertia,
    double* out_confidence
);

/* 包括的慣性計算（全レイヤー統合） */
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
    double* out_layer_breakdown,  /* [4] 各層の寄与 */
    char* out_explanation,        /* 説明文字列バッファ */
    int32_t explanation_size
);

/* ========================================
 * システム評価API（メイン機能）
 * ======================================== */

/* 汎用システム評価 */
SSD_UNIVERSAL_API SSDReturnCode ssd_evaluate_universal_system(
    SSDUniversalEngine* engine,
    const SSDUniversalStructure* structures,
    int32_t structure_count,
    const SSDUniversalMeaningPressure* meaning_pressures,
    int32_t pressure_count,
    const SSDEvaluationContext* context,
    SSDUniversalEvaluationResult* out_result
);

/* バッチ評価 */
SSD_UNIVERSAL_API SSDReturnCode ssd_evaluate_batch_systems(
    SSDUniversalEngine* engine,
    const SSDUniversalStructure** structure_arrays,
    const int32_t* structure_counts,
    const SSDUniversalMeaningPressure** pressure_arrays,
    const int32_t* pressure_counts,
    const SSDEvaluationContext* contexts,
    int32_t batch_size,
    SSDUniversalEvaluationResult* out_results
);

/* ========================================
 * 特殊化API（高レベル便利関数）
 * ======================================== */

/* NPCアクション評価（ゲーム特化） */
SSD_UNIVERSAL_API SSDReturnCode ssd_evaluate_npc_action(
    SSDUniversalEngine* engine,
    const char* action_id,
    const char* entity_id,
    double basal_drives[8],        /* 最大8種の基層衝動 */
    int32_t basal_count,
    double routine_strengths[16],  /* 最大16種の習慣強度 */
    int32_t routine_count,
    double episodic_influences[8], /* 最大8種の記憶影響 */
    int32_t episodic_count,
    double environmental_factors[8],
    int32_t env_count,
    double* out_action_inertia,
    double* out_confidence,
    char* out_reasoning,           /* 判断理由 */
    int32_t reasoning_size
);

/* 物理システム評価（科学計算特化） */
SSD_UNIVERSAL_API SSDReturnCode ssd_evaluate_physical_system(
    SSDUniversalEngine* engine,
    double system_stability,
    double system_complexity,
    double external_forces[8],
    int32_t force_count,
    double environmental_conditions[8],
    int32_t condition_count,
    SSDScale scale_level,
    double time_scale,
    double* out_stability_prediction,
    double* out_jump_probability,
    double* out_evolution_potential
);

/* 社会システム評価（社会科学特化） */
SSD_UNIVERSAL_API SSDReturnCode ssd_evaluate_social_system(
    SSDUniversalEngine* engine,
    double institutional_stability,
    double social_complexity,
    double external_pressures[8],
    int32_t pressure_count,
    double cultural_factors[8],
    int32_t cultural_count,
    int32_t population_size,
    double* out_system_health,
    double* out_change_probability,
    double* out_resilience
);

/* ========================================
 * ストリーミング・リアルタイムAPI
 * ======================================== */

/* コールバック関数型定義 */
typedef void (*SSDStreamingCallback)(const SSDUniversalEvaluationResult* result, void* user_data);

/* ストリーミング評価開始/停止 */
SSD_UNIVERSAL_API SSDReturnCode ssd_start_streaming(
    SSDUniversalEngine* engine,
    SSDStreamingCallback callback,
    void* user_data
);

SSD_UNIVERSAL_API SSDReturnCode ssd_stop_streaming(SSDUniversalEngine* engine);

/* リアルタイム評価（ストリーミング中に呼び出し） */
SSD_UNIVERSAL_API SSDReturnCode ssd_stream_evaluate(
    SSDUniversalEngine* engine,
    const SSDUniversalStructure* structures,
    int32_t structure_count,
    const SSDUniversalMeaningPressure* meaning_pressures,
    int32_t pressure_count,
    const SSDEvaluationContext* context
);

/* ========================================
 * JSON互換API（スクリプト言語向け）
 * ======================================== */

/* JSON入出力（Pythonとの互換性） */
SSD_UNIVERSAL_API SSDReturnCode ssd_evaluate_json(
    SSDUniversalEngine* engine,
    const char* input_json,
    char* output_json,
    int32_t output_size
);

/* JSONからネイティブ構造体への変換ヘルパー */
SSD_UNIVERSAL_API SSDReturnCode ssd_parse_json_to_structures(
    const char* json_input,
    SSDUniversalStructure* out_structures,
    int32_t max_structures,
    int32_t* out_count
);

SSD_UNIVERSAL_API SSDReturnCode ssd_parse_json_to_pressures(
    const char* json_input,
    SSDUniversalMeaningPressure* out_pressures,
    int32_t max_pressures,
    int32_t* out_count
);

/* ========================================
 * デバッグ・ユーティリティAPI
 * ======================================== */

/* システム状態ダンプ */
SSD_UNIVERSAL_API SSDReturnCode ssd_dump_engine_state(
    SSDUniversalEngine* engine,
    char* output_buffer,
    int32_t buffer_size
);

/* バージョン情報 */
SSD_UNIVERSAL_API const char* ssd_get_version_string(void);
SSD_UNIVERSAL_API void ssd_get_version_info(int32_t* major, int32_t* minor, int32_t* patch);

/* エラーメッセージ取得 */
SSD_UNIVERSAL_API const char* ssd_get_last_error_message(SSDUniversalEngine* engine);

/* メモリ使用量チェック */
SSD_UNIVERSAL_API double ssd_get_memory_usage_mb(SSDUniversalEngine* engine);

/* ========================================
 * 警告・推奨フラグ定義
 * ======================================== */

/* 警告フラグ */
#define SSD_WARNING_LOW_HEALTH         0x0001
#define SSD_WARNING_HIGH_JUMP_RISK     0x0002
#define SSD_WARNING_LOW_RESILIENCE     0x0004
#define SSD_WARNING_UNSTABLE_EVOLUTION 0x0008
#define SSD_WARNING_LOW_CONFIDENCE     0x0010
#define SSD_WARNING_HIGH_COMPLEXITY    0x0020

/* 推奨フラグ */
#define SSD_RECOMMEND_STABILIZE        0x0001
#define SSD_RECOMMEND_REDUCE_PRESSURE  0x0002
#define SSD_RECOMMEND_INCREASE_CHANGE  0x0004
#define SSD_RECOMMEND_ADD_REDUNDANCY   0x0008
#define SSD_RECOMMEND_OPTIMIZE_PARAMS  0x0010
#define SSD_RECOMMEND_MONITOR_CLOSELY  0x0020

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SSD_UNIVERSAL_ENGINE_DLL_H */