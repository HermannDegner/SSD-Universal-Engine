/*
 * test_basic.cpp
 * SSD Universal Engine基本テスト
 */

#include "ssd_universal_engine_dll.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <chrono>

void print_test_header(const char* test_name) {
    std::cout << "\n=== " << test_name << " ===" << std::endl;
}

void print_result(SSDReturnCode code, const char* operation) {
    std::cout << operation << ": ";
    switch (code) {
        case SSD_SUCCESS:
            std::cout << "SUCCESS" << std::endl;
            break;
        case SSD_WARNING_LOW_CONFIDENCE:
            std::cout << "WARNING (Low Confidence)" << std::endl;
            break;
        default:
            std::cout << "ERROR (" << (int)code << ")" << std::endl;
            break;
    }
}

int test_engine_lifecycle() {
    print_test_header("Engine Lifecycle Test");
    
    // デフォルト設定でエンジン作成
    SSDUniversalEngine* engine = ssd_universal_create(nullptr);
    if (!engine) {
        std::cout << "ERROR: Failed to create engine" << std::endl;
        return 1;
    }
    std::cout << "Engine created successfully" << std::endl;
    
    // バージョン情報取得
    std::cout << "Version: " << ssd_get_version_string() << std::endl;
    
    int major, minor, patch;
    ssd_get_version_info(&major, &minor, &patch);
    std::cout << "Version info: " << major << "." << minor << "." << patch << std::endl;
    
    // 統計情報取得
    SSDEngineStats stats;
    SSDReturnCode result = ssd_universal_get_stats(engine, &stats);
    print_result(result, "Get stats");
    
    if (result == SSD_SUCCESS) {
        std::cout << "Engine ID: " << stats.engine_id << std::endl;
        std::cout << "Total evaluations: " << stats.total_evaluations << std::endl;
        std::cout << "Memory usage: " << ssd_get_memory_usage_mb(engine) << " MB" << std::endl;
    }
    
    // エンジン破棄
    ssd_universal_destroy(engine);
    std::cout << "Engine destroyed successfully" << std::endl;
    
    return 0;
}

int test_basic_evaluation() {
    print_test_header("Basic System Evaluation Test");
    
    SSDEngineConfig config;
    memset(&config, 0, sizeof(config));
    config.precision_level = 2; // high
    config.calculation_mode = 1; // balanced
    config.enable_cache = 1;
    config.enable_prediction = 1;
    config.enable_explanation = 1;
    config.max_iterations = 1000;
    config.convergence_threshold = 1e-6;
    config.time_limit_ms = 5000;
    
    // 構造層重みの設定
    config.layer_weights[0] = 1.0; // physical
    config.layer_weights[1] = 0.9; // basal
    config.layer_weights[2] = 0.7; // core
    config.layer_weights[3] = 0.5; // upper
    
    SSDUniversalEngine* engine = ssd_universal_create(&config);
    if (!engine) {
        std::cout << "ERROR: Failed to create engine with config" << std::endl;
        return 1;
    }
    
    // テスト用構造定義
    SSDUniversalStructure structure;
    memset(&structure, 0, sizeof(structure));
    strncpy(structure.structure_id, "test_structure_1", sizeof(structure.structure_id) - 1);
    strncpy(structure.structure_type, "biological", sizeof(structure.structure_type) - 1);
    structure.dimension_count = 3;
    structure.stability_index = 0.7;
    structure.complexity_level = 0.6;
    structure.dynamic_count = 2;
    structure.dynamic_properties[0] = 0.5; // 特性1
    structure.dynamic_properties[1] = 0.8; // 特性2
    structure.constraint_rows = 2;
    structure.constraint_cols = 2;
    structure.constraint_matrix[0] = 0.8; // [0,0]
    structure.constraint_matrix[1] = 0.2; // [0,1]
    structure.constraint_matrix[2] = 0.3; // [1,0]
    structure.constraint_matrix[3] = 0.9; // [1,1]
    
    // テスト用意味圧定義
    SSDUniversalMeaningPressure pressure;
    memset(&pressure, 0, sizeof(pressure));
    strncpy(pressure.pressure_id, "test_pressure_1", sizeof(pressure.pressure_id) - 1);
    strncpy(pressure.source_type, "external", sizeof(pressure.source_type) - 1);
    pressure.magnitude = 0.6;
    pressure.direction_dims = 3;
    pressure.direction_vector[0] = 1.0;
    pressure.direction_vector[1] = 0.5;
    pressure.direction_vector[2] = 0.0;
    pressure.frequency = 0.1;
    pressure.duration = 3600;
    pressure.propagation_speed = 1.0;
    pressure.decay_function = 1; // exponential
    pressure.interaction_rows = 2;
    pressure.interaction_cols = 2;
    pressure.interaction_matrix[0] = 0.8;
    pressure.interaction_matrix[1] = 0.2;
    pressure.interaction_matrix[2] = 0.4;
    pressure.interaction_matrix[3] = 0.6;
    
    // 評価コンテキスト定義
    SSDEvaluationContext context;
    memset(&context, 0, sizeof(context));
    strncpy(context.context_id, "test_context_1", sizeof(context.context_id) - 1);
    context.domain = SSD_DOMAIN_BIOLOGY;
    context.scale_level = SSD_SCALE_CELLULAR;
    context.time_scale = 3600;
    context.space_scale = 1e-6;
    context.observer_position[0] = 0.0;
    context.observer_position[1] = 0.0;
    context.observer_position[2] = 0.0;
    context.measurement_precision = 0.95;
    context.env_factor_count = 2;
    context.environmental_factors[0] = 0.7; // 温度
    context.environmental_factors[1] = 0.6; // pH
    
    // システム評価実行
    SSDUniversalEvaluationResult result;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    SSDReturnCode eval_result = ssd_evaluate_universal_system(
        engine, &structure, 1, &pressure, 1, &context, &result
    );
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double elapsed_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    
    print_result(eval_result, "System evaluation");
    
    if (eval_result == SSD_SUCCESS || eval_result == SSD_WARNING_LOW_CONFIDENCE) {
        std::cout << "\n--- Evaluation Results ---" << std::endl;
        std::cout << "Evaluation ID: " << result.evaluation_id << std::endl;
        std::cout << "System Health: " << result.system_health << std::endl;
        std::cout << "Evolution Potential: " << result.evolution_potential << std::endl;
        std::cout << "Stability Resilience: " << result.stability_resilience << std::endl;
        std::cout << "Jump Probability: " << result.jump_probability << std::endl;
        std::cout << "Calculation Confidence: " << result.calculation_confidence << std::endl;
        std::cout << "Computational Cost: " << result.computational_cost << " seconds" << std::endl;
        std::cout << "Actual elapsed time: " << elapsed_ms << " ms" << std::endl;
        std::cout << "Prediction Horizon: " << result.prediction_horizon << " seconds" << std::endl;
        
        std::cout << "\n--- Structure Analysis ---" << std::endl;
        std::cout << "Stability: " << result.structure_stability << std::endl;
        std::cout << "Complexity: " << result.structure_complexity << std::endl;
        std::cout << "Adaptability: " << result.structure_adaptability << std::endl;
        
        std::cout << "\n--- Pressure Analysis ---" << std::endl;
        std::cout << "Magnitude: " << result.pressure_magnitude << std::endl;
        std::cout << "Coherence: " << result.pressure_coherence << std::endl;
        std::cout << "Sustainability: " << result.pressure_sustainability << std::endl;
        
        std::cout << "\n--- Alignment Analysis ---" << std::endl;
        std::cout << "Strength: " << result.alignment_strength << std::endl;
        std::cout << "Efficiency: " << result.alignment_efficiency << std::endl;
        std::cout << "Durability: " << result.alignment_durability << std::endl;
        
        std::cout << "\n--- Jump Analysis ---" << std::endl;
        std::cout << "Probability: " << result.jump_probability << std::endl;
        std::cout << "Direction: [";
        for (int i = 0; i < result.jump_direction_dims; i++) {
            std::cout << result.jump_direction[i];
            if (i < result.jump_direction_dims - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        std::cout << "Impact Estimation: " << result.jump_impact_estimation << std::endl;
        
        std::cout << "\n--- Warnings & Recommendations ---" << std::endl;
        std::cout << "Warning Flags: 0x" << std::hex << result.warning_flags << std::dec << std::endl;
        std::cout << "Recommendation Flags: 0x" << std::hex << result.recommendation_flags << std::dec << std::endl;
        
        std::cout << "\n--- Explanation ---" << std::endl;
        std::cout << result.explanation_json << std::endl;
    } else {
        std::cout << "Evaluation failed. Error: " << ssd_get_last_error_message(engine) << std::endl;
    }
    
    ssd_universal_destroy(engine);
    return (eval_result == SSD_SUCCESS || eval_result == SSD_WARNING_LOW_CONFIDENCE) ? 0 : 1;
}

int test_npc_evaluation() {
    print_test_header("NPC Action Evaluation Test");
    
    SSDUniversalEngine* engine = ssd_universal_create(nullptr);
    if (!engine) {
        std::cout << "ERROR: Failed to create engine" << std::endl;
        return 1;
    }
    
    // NPC基層衝動（どうぶつの森風）
    double basal_drives[5] = {
        0.3,  // 安心欲求
        0.8,  // 社交欲求  
        0.4,  // 探索欲求
        0.2,  // 創造欲求
        0.6   // 承認欲求
    };
    
    // 習慣強度
    double routine_strengths[4] = {
        0.9,  // 朝の散歩
        0.7,  // 挨拶パターン
        0.5,  // 読書習慣
        0.3   // 掃除ルーチン
    };
    
    // エピソード記憶の影響
    double episodic_influences[3] = {
        0.8,  // プレイヤーAとのポジティブな記憶
        -0.2, // プレイヤーBとのネガティブな記憶
        0.6   // 友達との楽しい思い出
    };
    
    // 環境要因
    double environmental_factors[4] = {
        0.9,  // 天気（晴天）
        0.7,  // 時間帯（朝）
        0.5,  // 周囲の活動レベル
        0.8   // 安全性
    };
    
    double action_inertia, confidence;
    char reasoning[512];
    
    SSDReturnCode result = ssd_evaluate_npc_action(
        engine,
        "greet_player",
        "PlayerA", 
        basal_drives, 5,
        routine_strengths, 4,
        episodic_influences, 3,
        environmental_factors, 4,
        &action_inertia,
        &confidence,
        reasoning, sizeof(reasoning)
    );
    
    print_result(result, "NPC action evaluation");
    
    if (result == SSD_SUCCESS || result == SSD_WARNING_LOW_CONFIDENCE) {
        std::cout << "Action: greet_player" << std::endl;
        std::cout << "Target: PlayerA" << std::endl;
        std::cout << "Action Inertia: " << action_inertia << std::endl;
        std::cout << "Confidence: " << confidence << std::endl;
        std::cout << "Reasoning: " << reasoning << std::endl;
        
        // 閾値による行動判定例
        if (action_inertia > 0.6) {
            std::cout << "Decision: EXECUTE ACTION (high inertia)" << std::endl;
        } else if (action_inertia > 0.3) {
            std::cout << "Decision: CONSIDER ACTION (medium inertia)" << std::endl;
        } else {
            std::cout << "Decision: AVOID ACTION (low inertia)" << std::endl;
        }
    } else {
        std::cout << "NPC evaluation failed. Error: " << ssd_get_last_error_message(engine) << std::endl;
    }
    
    ssd_universal_destroy(engine);
    return (result == SSD_SUCCESS || result == SSD_WARNING_LOW_CONFIDENCE) ? 0 : 1;
}

int main() {
    std::cout << "SSD Universal Engine - Basic Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // テスト実行
    total_tests++;
    if (test_engine_lifecycle() == 0) passed_tests++;
    
    total_tests++;
    if (test_basic_evaluation() == 0) passed_tests++;
    
    total_tests++;
    if (test_npc_evaluation() == 0) passed_tests++;
    
    // 結果サマリー
    std::cout << "\n========================================" << std::endl;
    std::cout << "Test Results: " << passed_tests << "/" << total_tests << " passed" << std::endl;
    
    if (passed_tests == total_tests) {
        std::cout << "All tests PASSED! ✅" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests FAILED! ❌" << std::endl;
        return 1;
    }
}