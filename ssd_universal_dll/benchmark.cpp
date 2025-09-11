/*
 * benchmark.cpp
 * SSD Universal Engine パフォーマンス・ベンチマーク
 */

#include "ssd_universal_engine_dll.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <cstring>
#include <random>
#include <iomanip>

class Timer {
public:
    Timer() { start(); }
    
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    double elapsed_ms() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }
    
    double elapsed_seconds() const {
        return elapsed_ms() / 1000.0;
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_time;
};

void print_benchmark_header(const char* test_name) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "🚀 " << test_name << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

void print_performance_stats(const std::vector<double>& times, const char* operation) {
    if (times.empty()) return;
    
    double total = 0.0;
    double min_time = times[0];
    double max_time = times[0];
    
    for (double t : times) {
        total += t;
        if (t < min_time) min_time = t;
        if (t > max_time) max_time = t;
    }
    
    double avg = total / times.size();
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << operation << " Performance:" << std::endl;
    std::cout << "  Samples: " << times.size() << std::endl;
    std::cout << "  Average: " << avg << " ms" << std::endl;
    std::cout << "  Min:     " << min_time << " ms" << std::endl;
    std::cout << "  Max:     " << max_time << " ms" << std::endl;
    std::cout << "  Total:   " << total << " ms" << std::endl;
    std::cout << "  Rate:    " << (times.size() / (total / 1000.0)) << " ops/sec" << std::endl;
}

int benchmark_basic_evaluation() {
    print_benchmark_header("Basic System Evaluation Benchmark");
    
    SSDUniversalEngine* engine = ssd_universal_create(nullptr);
    if (!engine) {
        std::cout << "ERROR: Failed to create engine" << std::endl;
        return 1;
    }
    
    // テスト用データ準備
    SSDUniversalStructure structure;
    memset(&structure, 0, sizeof(structure));
    strncpy(structure.structure_id, "bench_structure", sizeof(structure.structure_id) - 1);
    strncpy(structure.structure_type, "test", sizeof(structure.structure_type) - 1);
    structure.dimension_count = 3;
    structure.stability_index = 0.7;
    structure.complexity_level = 0.6;
    structure.dynamic_count = 2;
    structure.dynamic_properties[0] = 0.5;
    structure.dynamic_properties[1] = 0.8;
    
    SSDUniversalMeaningPressure pressure;
    memset(&pressure, 0, sizeof(pressure));
    strncpy(pressure.pressure_id, "bench_pressure", sizeof(pressure.pressure_id) - 1);
    strncpy(pressure.source_type, "external", sizeof(pressure.source_type) - 1);
    pressure.magnitude = 0.6;
    pressure.direction_dims = 3;
    pressure.direction_vector[0] = 1.0;
    pressure.direction_vector[1] = 0.5;
    pressure.direction_vector[2] = 0.0;
    pressure.frequency = 0.1;
    pressure.duration = 3600;
    
    SSDEvaluationContext context;
    memset(&context, 0, sizeof(context));
    strncpy(context.context_id, "bench_context", sizeof(context.context_id) - 1);
    context.domain = SSD_DOMAIN_AI;
    context.scale_level = SSD_SCALE_ORGANISM;
    context.time_scale = 1.0;
    context.space_scale = 1.0;
    context.measurement_precision = 0.8;
    
    // ベンチマーク実行
    const int num_iterations = 1000;
    std::vector<double> evaluation_times;
    
    std::cout << "Running " << num_iterations << " evaluations..." << std::endl;
    
    Timer total_timer;
    
    for (int i = 0; i < num_iterations; i++) {
        Timer timer;
        
        SSDUniversalEvaluationResult result;
        SSDReturnCode ret = ssd_evaluate_universal_system(
            engine, &structure, 1, &pressure, 1, &context, &result
        );
        
        evaluation_times.push_back(timer.elapsed_ms());
        
        if (ret != SSD_SUCCESS && ret != SSD_WARNING_LOW_CONFIDENCE) {
            std::cout << "ERROR: Evaluation failed at iteration " << i << std::endl;
            ssd_universal_destroy(engine);
            return 1;
        }
        
        // プログレス表示
        if ((i + 1) % 100 == 0) {
            std::cout << "  Completed: " << (i + 1) << "/" << num_iterations 
                      << " (" << std::fixed << std::setprecision(1) 
                      << (100.0 * (i + 1) / num_iterations) << "%)" << std::endl;
        }
    }
    
    double total_time = total_timer.elapsed_seconds();
    
    print_performance_stats(evaluation_times, "System Evaluation");
    
    // エンジン統計
    SSDEngineStats stats;
    ssd_universal_get_stats(engine, &stats);
    
    std::cout << "\nEngine Statistics:" << std::endl;
    std::cout << "  Total evaluations: " << stats.total_evaluations << std::endl;
    std::cout << "  Cache hit rate: " << std::fixed << std::setprecision(1) 
              << (stats.cache_hit_rate * 100) << "%" << std::endl;
    std::cout << "  Memory usage: " << std::setprecision(2) 
              << ssd_get_memory_usage_mb(engine) << " MB" << std::endl;
    
    ssd_universal_destroy(engine);
    return 0;
}

int benchmark_npc_evaluation() {
    print_benchmark_header("NPC Action Evaluation Benchmark");
    
    SSDUniversalEngine* engine = ssd_universal_create(nullptr);
    if (!engine) {
        std::cout << "ERROR: Failed to create engine" << std::endl;
        return 1;
    }
    
    // ランダムな NPC パラメータ生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    const int num_npcs = 100;
    const int actions_per_npc = 10;
    
    std::vector<double> npc_eval_times;
    
    std::cout << "Testing " << num_npcs << " NPCs with " << actions_per_npc 
              << " actions each (" << (num_npcs * actions_per_npc) << " total evaluations)..." << std::endl;
    
    Timer total_timer;
    
    for (int npc = 0; npc < num_npcs; npc++) {
        // ランダムな NPC パラメータ生成
        double basal_drives[6];
        double routine_strengths[8];
        double episodic_influences[4];
        double environmental_factors[4];
        
        for (int i = 0; i < 6; i++) basal_drives[i] = dis(gen);
        for (int i = 0; i < 8; i++) routine_strengths[i] = dis(gen);
        for (int i = 0; i < 4; i++) episodic_influences[i] = dis(gen) * 2.0 - 1.0; // -1 to 1
        for (int i = 0; i < 4; i++) environmental_factors[i] = dis(gen);
        
        char npc_id[32];
        snprintf(npc_id, sizeof(npc_id), "NPC_%03d", npc);
        
        for (int action = 0; action < actions_per_npc; action++) {
            Timer timer;
            
            char action_id[32];
            snprintf(action_id, sizeof(action_id), "action_%d", action);
            
            double action_inertia, confidence;
            char reasoning[256];
            
            SSDReturnCode ret = ssd_evaluate_npc_action(
                engine,
                action_id,
                npc_id,
                basal_drives, 6,
                routine_strengths, 8,
                episodic_influences, 4,
                environmental_factors, 4,
                &action_inertia,
                &confidence,
                reasoning, sizeof(reasoning)
            );
            
            npc_eval_times.push_back(timer.elapsed_ms());
            
            if (ret != SSD_SUCCESS && ret != SSD_WARNING_LOW_CONFIDENCE) {
                std::cout << "ERROR: NPC evaluation failed for " << npc_id << std::endl;
                ssd_universal_destroy(engine);
                return 1;
            }
        }
        
        // プログレス表示
        if ((npc + 1) % 10 == 0) {
            std::cout << "  Completed NPCs: " << (npc + 1) << "/" << num_npcs 
                      << " (" << std::fixed << std::setprecision(1) 
                      << (100.0 * (npc + 1) / num_npcs) << "%)" << std::endl;
        }
    }
    
    print_performance_stats(npc_eval_times, "NPC Action Evaluation");
    
    ssd_universal_destroy(engine);
    return 0;
}

int benchmark_comprehensive_inertia() {
    print_benchmark_header("Comprehensive Inertia Calculation Benchmark");
    
    SSDUniversalEngine* engine = ssd_universal_create(nullptr);
    if (!engine) {
        std::cout << "ERROR: Failed to create engine" << std::endl;
        return 1;
    }
    
    // コンポーネント準備
    SSDInertiaComponent basal_components[4];
    SSDInertiaComponent core_components[6];
    SSDInertiaComponent upper_components[3];
    
    // ダミーデータ初期化
    for (int i = 0; i < 4; i++) {
        memset(&basal_components[i], 0, sizeof(SSDInertiaComponent));
        snprintf(basal_components[i].component_id, sizeof(basal_components[i].component_id), "basal_%d", i);
        basal_components[i].base_strength = 0.5 + (i * 0.1);
        basal_components[i].usage_frequency = 0.8;
        basal_components[i].success_rate = 0.85;
        basal_components[i].temporal_stability = 0.9;
        basal_components[i].reinforcement_count = 100;
        basal_components[i].decay_resistance = 0.95;
    }
    
    for (int i = 0; i < 6; i++) {
        memset(&core_components[i], 0, sizeof(SSDInertiaComponent));
        snprintf(core_components[i].component_id, sizeof(core_components[i].component_id), "core_%d", i);
        core_components[i].base_strength = 0.6 + (i * 0.05);
        core_components[i].usage_frequency = 0.7;
        core_components[i].success_rate = 0.9;
        core_components[i].temporal_stability = 0.85;
        core_components[i].reinforcement_count = 150;
        core_components[i].decay_resistance = 0.9;
    }
    
    for (int i = 0; i < 3; i++) {
        memset(&upper_components[i], 0, sizeof(SSDInertiaComponent));
        snprintf(upper_components[i].component_id, sizeof(upper_components[i].component_id), "upper_%d", i);
        upper_components[i].base_strength = 0.7 + (i * 0.1);
        upper_components[i].usage_frequency = 0.5;
        upper_components[i].success_rate = 0.75;
        upper_components[i].temporal_stability = 0.7;
        upper_components[i].reinforcement_count = 50;
        upper_components[i].decay_resistance = 0.8;
    }
    
    SSDEvaluationContext context;
    memset(&context, 0, sizeof(context));
    strncpy(context.context_id, "inertia_bench", sizeof(context.context_id) - 1);
    context.domain = SSD_DOMAIN_AI;
    context.scale_level = SSD_SCALE_ORGANISM;
    context.time_scale = 1.0;
    context.space_scale = 1.0;
    context.measurement_precision = 0.9;
    
    const int num_iterations = 500;
    std::vector<double> inertia_times;
    
    std::cout << "Running " << num_iterations << " comprehensive inertia calculations..." << std::endl;
    
    for (int i = 0; i < num_iterations; i++) {
        Timer timer;
        
        double total_inertia;
        double layer_breakdown[4];
        char explanation[512];
        
        SSDReturnCode ret = ssd_calculate_comprehensive_inertia(
            engine,
            nullptr, 0,              // physical
            basal_components, 4,     // basal
            core_components, 6,      // core
            upper_components, 3,     // upper
            &context,
            &total_inertia,
            layer_breakdown,
            explanation, sizeof(explanation)
        );
        
        inertia_times.push_back(timer.elapsed_ms());
        
        if (ret != SSD_SUCCESS && ret != SSD_WARNING_LOW_CONFIDENCE) {
            std::cout << "ERROR: Inertia calculation failed at iteration " << i << std::endl;
            ssd_universal_destroy(engine);
            return 1;
        }
        
        // プログレス表示
        if ((i + 1) % 50 == 0) {
            std::cout << "  Completed: " << (i + 1) << "/" << num_iterations 
                      << " (" << std::fixed << std::setprecision(1) 
                      << (100.0 * (i + 1) / num_iterations) << "%)" << std::endl;
        }
    }
    
    print_performance_stats(inertia_times, "Comprehensive Inertia Calculation");
    
    ssd_universal_destroy(engine);
    return 0;
}

int benchmark_cache_effectiveness() {
    print_benchmark_header("Cache Effectiveness Benchmark");
    
    // キャッシュ有効時のテスト
    SSDEngineConfig config_with_cache;
    memset(&config_with_cache, 0, sizeof(config_with_cache));
    config_with_cache.precision_level = 2;
    config_with_cache.calculation_mode = 1;
    config_with_cache.enable_cache = 1; // キャッシュ有効
    config_with_cache.enable_prediction = 1;
    config_with_cache.enable_explanation = 1;
    
    // キャッシュ無効時のテスト
    SSDEngineConfig config_without_cache = config_with_cache;
    config_without_cache.enable_cache = 0; // キャッシュ無効
    
    // テストデータ
    SSDUniversalStructure structure;
    SSDUniversalMeaningPressure pressure;
    SSDEvaluationContext context;
    
    memset(&structure, 0, sizeof(structure));
    strncpy(structure.structure_id, "cache_test_struct", sizeof(structure.structure_id) - 1);
    structure.stability_index = 0.7;
    structure.complexity_level = 0.6;
    
    memset(&pressure, 0, sizeof(pressure));
    strncpy(pressure.pressure_id, "cache_test_pressure", sizeof(pressure.pressure_id) - 1);
    pressure.magnitude = 0.6;
    
    memset(&context, 0, sizeof(context));
    strncpy(context.context_id, "cache_test_context", sizeof(context.context_id) - 1);
    context.domain = SSD_DOMAIN_AI;
    context.scale_level = SSD_SCALE_ORGANISM;
    context.measurement_precision = 0.8;
    
    const int num_iterations = 200;
    const int num_repeats = 5; // 同じクエリを5回繰り返し
    
    // キャッシュ有効時のテスト
    std::cout << "Testing WITH cache..." << std::endl;
    SSDUniversalEngine* engine_with_cache = ssd_universal_create(&config_with_cache);
    if (!engine_with_cache) {
        std::cout << "ERROR: Failed to create engine with cache" << std::endl;
        return 1;
    }
    
    std::vector<double> cached_times;
    Timer cached_timer;
    
    for (int i = 0; i < num_iterations; i++) {
        for (int repeat = 0; repeat < num_repeats; repeat++) {
            Timer timer;
            SSDUniversalEvaluationResult result;
            
            SSDReturnCode ret = ssd_evaluate_universal_system(
                engine_with_cache, &structure, 1, &pressure, 1, &context, &result
            );
            
            cached_times.push_back(timer.elapsed_ms());
            
            if (ret != SSD_SUCCESS && ret != SSD_WARNING_LOW_CONFIDENCE) {
                std::cout << "ERROR: Cached evaluation failed" << std::endl;
                ssd_universal_destroy(engine_with_cache);
                return 1;
            }
        }
        
        // 少しパラメータを変更して新しいクエリも混ぜる
        structure.stability_index = 0.7 + (i % 10) * 0.01;
    }
    
    SSDEngineStats cached_stats;
    ssd_universal_get_stats(engine_with_cache, &cached_stats);
    ssd_universal_destroy(engine_with_cache);
    
    // キャッシュ無効時のテスト
    std::cout << "Testing WITHOUT cache..." << std::endl;
    SSDUniversalEngine* engine_without_cache = ssd_universal_create(&config_without_cache);
    if (!engine_without_cache) {
        std::cout << "ERROR: Failed to create engine without cache" << std::endl;
        return 1;
    }
    
    std::vector<double> non_cached_times;
    Timer non_cached_timer;
    
    structure.stability_index = 0.7; // リセット
    
    for (int i = 0; i < num_iterations; i++) {
        for (int repeat = 0; repeat < num_repeats; repeat++) {
            Timer timer;
            SSDUniversalEvaluationResult result;
            
            SSDReturnCode ret = ssd_evaluate_universal_system(
                engine_without_cache, &structure, 1, &pressure, 1, &context, &result
            );
            
            non_cached_times.push_back(timer.elapsed_ms());
            
            if (ret != SSD_SUCCESS && ret != SSD_WARNING_LOW_CONFIDENCE) {
                std::cout << "ERROR: Non-cached evaluation failed" << std::endl;
                ssd_universal_destroy(engine_without_cache);
                return 1;
            }
        }
        
        structure.stability_index = 0.7 + (i % 10) * 0.01;
    }
    
    SSDEngineStats non_cached_stats;
    ssd_universal_get_stats(engine_without_cache, &non_cached_stats);
    ssd_universal_destroy(engine_without_cache);
    
    // 結果比較
    print_performance_stats(cached_times, "WITH Cache");
    print_performance_stats(non_cached_times, "WITHOUT Cache");
    
    std::cout << "\nCache Effectiveness Analysis:" << std::endl;
    std::cout << "  Cache hit rate: " << std::fixed << std::setprecision(1) 
              << (cached_stats.cache_hit_rate * 100) << "%" << std::endl;
    
    double cached_avg = 0.0, non_cached_avg = 0.0;
    for (double t : cached_times) cached_avg += t;
    for (double t : non_cached_times) non_cached_avg += t;
    cached_avg /= cached_times.size();
    non_cached_avg /= non_cached_times.size();
    
    double speedup = non_cached_avg / cached_avg;
    std::cout << "  Speedup factor: " << std::setprecision(2) << speedup << "x" << std::endl;
    std::cout << "  Time saved: " << std::setprecision(1) 
              << ((1.0 - 1.0/speedup) * 100) << "%" << std::endl;
    
    return 0;
}

int benchmark_memory_usage() {
    print_benchmark_header("Memory Usage Analysis");
    
    SSDUniversalEngine* engine = ssd_universal_create(nullptr);
    if (!engine) {
        std::cout << "ERROR: Failed to create engine" << std::endl;
        return 1;
    }
    
    std::cout << "Initial memory usage: " << std::fixed << std::setprecision(2) 
              << ssd_get_memory_usage_mb(engine) << " MB" << std::endl;
    
    // メモリ使用量の増加を測定
    SSDUniversalStructure structure;
    SSDUniversalMeaningPressure pressure;
    SSDEvaluationContext context;
    
    memset(&structure, 0, sizeof(structure));
    memset(&pressure, 0, sizeof(pressure));
    memset(&context, 0, sizeof(context));
    
    strncpy(structure.structure_id, "memory_test", sizeof(structure.structure_id) - 1);
    strncpy(pressure.pressure_id, "memory_test", sizeof(pressure.pressure_id) - 1);
    strncpy(context.context_id, "memory_test", sizeof(context.context_id) - 1);
    
    context.domain = SSD_DOMAIN_AI;
    context.scale_level = SSD_SCALE_ORGANISM;
    structure.stability_index = 0.7;
    pressure.magnitude = 0.6;
    
    const int batches = 10;
    const int evaluations_per_batch = 100;
    
    for (int batch = 0; batch < batches; batch++) {
        // バッチごとに異なるIDでキャッシュを蓄積
        for (int i = 0; i < evaluations_per_batch; i++) {
            char unique_id[64];
            snprintf(unique_id, sizeof(unique_id), "memory_test_%d_%d", batch, i);
            strncpy(structure.structure_id, unique_id, sizeof(structure.structure_id) - 1);
            
            SSDUniversalEvaluationResult result;
            ssd_evaluate_universal_system(engine, &structure, 1, &pressure, 1, &context, &result);
        }
        
        double memory_mb = ssd_get_memory_usage_mb(engine);
        SSDEngineStats stats;
        ssd_universal_get_stats(engine, &stats);
        
        std::cout << "After batch " << (batch + 1) << ": " 
                  << memory_mb << " MB, " 
                  << stats.cache_size << " cached items, "
                  << stats.total_evaluations << " total evaluations" << std::endl;
    }
    
    ssd_universal_destroy(engine);
    return 0;
}

int main() {
    std::cout << "SSD Universal Engine - Performance Benchmark Suite" << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << "Testing system performance and scalability..." << std::endl;
    
    int total_benchmarks = 0;
    int passed_benchmarks = 0;
    
    // ベンチマーク実行
    total_benchmarks++;
    if (benchmark_basic_evaluation() == 0) passed_benchmarks++;
    
    total_benchmarks++;
    if (benchmark_npc_evaluation() == 0) passed_benchmarks++;
    
    total_benchmarks++;
    if (benchmark_comprehensive_inertia() == 0) passed_benchmarks++;
    
    total_benchmarks++;
    if (benchmark_cache_effectiveness() == 0) passed_benchmarks++;
    
    total_benchmarks++;
    if (benchmark_memory_usage() == 0) passed_benchmarks++;
    
    // 結果サマリー
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Benchmark Results: " << passed_benchmarks << "/" << total_benchmarks << " completed" << std::endl;
    
    if (passed_benchmarks == total_benchmarks) {
        std::cout << "All benchmarks COMPLETED! 🚀✅" << std::endl;
        std::cout << "System performance is within expected parameters." << std::endl;
        return 0;
    } else {
        std::cout << "Some benchmarks FAILED! 🚀❌" << std::endl;
        std::cout << "Performance issues detected." << std::endl;
        return 1;
    }
}