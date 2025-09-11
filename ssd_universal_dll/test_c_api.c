/*
 * test_c_api.c
 * SSD Universal Engine C API テスト
 */

#include "ssd_universal_engine_dll.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_test_header(const char* test_name) {
    printf("\n=== %s ===\n", test_name);
}

void print_result(SSDReturnCode code, const char* operation) {
    printf("%s: ", operation);
    switch (code) {
        case SSD_SUCCESS:
            printf("SUCCESS\n");
            break;
        case SSD_WARNING_LOW_CONFIDENCE:
            printf("WARNING (Low Confidence)\n");
            break;
        default:
            printf("ERROR (%d)\n", (int)code);
            break;
    }
}

int test_c_engine_basic() {
    print_test_header("C API Basic Test");
    
    /* エンジン作成 */
    SSDUniversalEngine* engine = ssd_universal_create(NULL);
    if (!engine) {
        printf("ERROR: Failed to create engine\n");
        return 1;
    }
    printf("Engine created successfully\n");
    
    /* バージョン情報 */
    printf("Version: %s\n", ssd_get_version_string());
    
    int major, minor, patch;
    ssd_get_version_info(&major, &minor, &patch);
    printf("Version info: %d.%d.%d\n", major, minor, patch);
    
    /* 統計情報取得 */
    SSDEngineStats stats;
    SSDReturnCode result = ssd_universal_get_stats(engine, &stats);
    print_result(result, "Get stats");
    
    if (result == SSD_SUCCESS) {
        printf("Engine ID: %s\n", stats.engine_id);
        printf("Total evaluations: %llu\n", (unsigned long long)stats.total_evaluations);
        printf("Memory usage: %.2f MB\n", ssd_get_memory_usage_mb(engine));
    }
    
    /* エンジン破棄 */
    ssd_universal_destroy(engine);
    printf("Engine destroyed successfully\n");
    
    return 0;
}

int test_c_npc_evaluation() {
    print_test_header("C API NPC Evaluation Test");
    
    SSDUniversalEngine* engine = ssd_universal_create(NULL);
    if (!engine) {
        printf("ERROR: Failed to create engine\n");
        return 1;
    }
    
    /* NPC基層衝動設定 */
    double basal_drives[5] = {0.3, 0.8, 0.4, 0.2, 0.6};
    double routine_strengths[3] = {0.9, 0.7, 0.5};
    double episodic_influences[2] = {0.8, -0.2};
    double environmental_factors[3] = {0.9, 0.7, 0.8};
    
    double action_inertia, confidence;
    char reasoning[256];
    
    SSDReturnCode result = ssd_evaluate_npc_action(
        engine,
        "greet_player",
        "PlayerA",
        basal_drives, 5,
        routine_strengths, 3,
        episodic_influences, 2,
        environmental_factors, 3,
        &action_inertia,
        &confidence,
        reasoning, sizeof(reasoning)
    );
    
    print_result(result, "NPC action evaluation");
    
    if (result == SSD_SUCCESS || result == SSD_WARNING_LOW_CONFIDENCE) {
        printf("Action: greet_player\n");
        printf("Target: PlayerA\n");
        printf("Action Inertia: %.3f\n", action_inertia);
        printf("Confidence: %.3f\n", confidence);
        printf("Reasoning: %s\n", reasoning);
        
        /* 行動判定 */
        if (action_inertia > 0.6) {
            printf("Decision: EXECUTE ACTION\n");
        } else if (action_inertia > 0.3) {
            printf("Decision: CONSIDER ACTION\n");
        } else {
            printf("Decision: AVOID ACTION\n");
        }
    } else {
        printf("NPC evaluation failed. Error: %s\n", ssd_get_last_error_message(engine));
    }
    
    ssd_universal_destroy(engine);
    return (result == SSD_SUCCESS || result == SSD_WARNING_LOW_CONFIDENCE) ? 0 : 1;
}

int test_c_comprehensive_inertia() {
    print_test_header("C API Comprehensive Inertia Test");
    
    SSDUniversalEngine* engine = ssd_universal_create(NULL);
    if (!engine) {
        printf("ERROR: Failed to create engine\n");
        return 1;
    }
    
    /* 基層コンポーネント作成 */
    SSDInertiaComponent basal_components[2];
    memset(basal_components, 0, sizeof(basal_components));
    
    /* コンポーネント1: 社交衝動 */
    strncpy(basal_components[0].component_id, "social_drive", sizeof(basal_components[0].component_id) - 1);
    basal_components[0].base_strength = 0.8;
    basal_components[0].usage_frequency = 0.9;
    basal_components[0].success_rate = 0.85;
    basal_components[0].last_activation = 1000; /* タイムスタンプ */
    basal_components[0].temporal_stability = 0.9;
    basal_components[0].reinforcement_count = 50;
    basal_components[0].decay_resistance = 0.95;
    basal_components[0].binding_count = 0;
    
    /* コンポーネント2: 安心欲求 */
    strncpy(basal_components[1].component_id, "comfort_need", sizeof(basal_components[1].component_id) - 1);
    basal_components[1].base_strength = 0.6;
    basal_components[1].usage_frequency = 0.7;
    basal_components[1].success_rate = 0.9;
    basal_components[1].last_activation = 2000;
    basal_components[1].temporal_stability = 0.95;
    basal_components[1].reinforcement_count = 100;
    basal_components[1].decay_resistance = 0.98;
    basal_components[1].binding_count = 0;
    
    /* 評価コンテキスト */
    SSDEvaluationContext context;
    memset(&context, 0, sizeof(context));
    strncpy(context.context_id, "test_context", sizeof(context.context_id) - 1);
    context.domain = SSD_DOMAIN_AI;
    context.scale_level = SSD_SCALE_ORGANISM;
    context.time_scale = 1.0;
    context.space_scale = 1.0;
    context.measurement_precision = 0.8;
    context.env_factor_count = 1;
    context.environmental_factors[0] = 0.7;
    
    /* 包括的慣性計算 */
    double total_inertia;
    double layer_breakdown[4];
    char explanation[256];
    
    SSDReturnCode result = ssd_calculate_comprehensive_inertia(
        engine,
        NULL, 0,                    /* physical */
        basal_components, 2,        /* basal */
        NULL, 0,                    /* core */
        NULL, 0,                    /* upper */
        &context,
        &total_inertia,
        layer_breakdown,
        explanation, sizeof(explanation)
    );
    
    print_result(result, "Comprehensive inertia calculation");
    
    if (result == SSD_SUCCESS || result == SSD_WARNING_LOW_CONFIDENCE) {
        printf("Total Inertia: %.3f\n", total_inertia);
        printf("Layer Breakdown:\n");
        printf("  Physical: %.3f\n", layer_breakdown[0]);
        printf("  Basal: %.3f\n", layer_breakdown[1]);
        printf("  Core: %.3f\n", layer_breakdown[2]);
        printf("  Upper: %.3f\n", layer_breakdown[3]);
        printf("Explanation: %s\n", explanation);
    } else {
        printf("Comprehensive inertia calculation failed. Error: %s\n", ssd_get_last_error_message(engine));
    }
    
    ssd_universal_destroy(engine);
    return (result == SSD_SUCCESS || result == SSD_WARNING_LOW_CONFIDENCE) ? 0 : 1;
}

int main() {
    printf("SSD Universal Engine - C API Test Suite\n");
    printf("========================================\n");
    
    int total_tests = 0;
    int passed_tests = 0;
    
    /* テスト実行 */
    total_tests++;
    if (test_c_engine_basic() == 0) passed_tests++;
    
    total_tests++;
    if (test_c_npc_evaluation() == 0) passed_tests++;
    
    total_tests++;
    if (test_c_comprehensive_inertia() == 0) passed_tests++;
    
    /* 結果サマリー */
    printf("\n========================================\n");
    printf("Test Results: %d/%d passed\n", passed_tests, total_tests);
    
    if (passed_tests == total_tests) {
        printf("All tests PASSED! ✅\n");
        return 0;
    } else {
        printf("Some tests FAILED! ❌\n");
        return 1;
    }
}