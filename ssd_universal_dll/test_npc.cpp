/*
 * test_npc.cpp
 * SSD Universal Engine NPC専用テスト
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

int test_villager_daily_routine() {
    print_test_header("Villager Daily Routine Test");
    
    SSDUniversalEngine* engine = ssd_universal_create(nullptr);
    if (!engine) {
        std::cout << "ERROR: Failed to create engine" << std::endl;
        return 1;
    }
    
    // どうぶつの森風住民「リリアン」のパラメータ
    double basal_drives[6] = {
        0.4,  // 安心欲求（内向的性格）
        0.9,  // 社交欲求（フレンドリー）
        0.3,  // 探索欲求（安定志向）
        0.7,  // 創造欲求（ガーデニング好き）
        0.6,  // 承認欲求（程よく）
        0.5   // 自立欲求（バランス）
    };
    
    double routine_strengths[8] = {
        0.95, // 朝の水やりルーチン
        0.8,  // 挨拶パターン
        0.7,  // お昼のお茶時間
        0.6,  // 夕方の散歩
        0.4,  // 読書習慣
        0.3,  // 掃除ルーチン
        0.85, // 花の手入れ
        0.5   // 料理準備
    };
    
    double episodic_influences[5] = {
        0.9,  // プレイヤーとの楽しい会話記憶
        0.7,  // 友達との花見の思い出
        -0.1, // 雨の日の軽いストレス
        0.8,  // 誕生日パーティーの記憶
        0.6   // 新しい花が咲いた喜び
    };
    
    // 時間帯・天候別テスト
    struct TestScenario {
        const char* scenario_name;
        const char* action;
        double env_factors[6];
        const char* expected_behavior;
    };
    
    TestScenario scenarios[] = {
        {
            "朝8時・晴天",
            "water_flowers",
            {0.9, 0.8, 0.6, 0.8, 0.2, 0.3}, // 晴天、朝、静か、安全、人少ない、エネルギー高
            "高い慣性（朝のルーチン）"
        },
        {
            "昼12時・曇り",
            "greet_player", 
            {0.6, 0.5, 0.8, 0.8, 0.7, 0.6}, // 曇り、昼、活動的、安全、人多い、普通
            "中程度の慣性（社交タイム）"
        },
        {
            "夕方18時・雨",
            "indoor_activity",
            {0.2, 0.7, 0.3, 0.9, 0.4, 0.4}, // 雨、夕方、静か、安全、人少ない、疲れ
            "中程度の慣性（室内活動）"
        },
        {
            "夜22時・晴天",
            "prepare_sleep",
            {0.8, 0.9, 0.2, 0.9, 0.1, 0.2}, // 晴天、夜、静か、安全、人いない、眠い
            "高い慣性（就寝準備）"
        }
    };
    
    std::cout << "住民「リリアン」の行動慣性分析" << std::endl;
    std::cout << "性格：フレンドリー、ガーデニング好き、安定志向" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    for (const auto& scenario : scenarios) {
        std::cout << "\n【" << scenario.scenario_name << "】" << std::endl;
        
        double action_inertia, confidence;
        char reasoning[512];
        
        SSDReturnCode result = ssd_evaluate_npc_action(
            engine,
            scenario.action,
            "Lillian",
            basal_drives, 6,
            routine_strengths, 8,
            episodic_influences, 5,
            const_cast<double*>(scenario.env_factors), 6,
            &action_inertia,
            &confidence,
            reasoning, sizeof(reasoning)
        );
        
        if (result == SSD_SUCCESS || result == SSD_WARNING_LOW_CONFIDENCE) {
            std::cout << "行動: " << scenario.action << std::endl;
            std::cout << "慣性値: " << std::fixed << std::setprecision(3) << action_inertia << std::endl;
            std::cout << "信頼度: " << confidence << std::endl;
            std::cout << "予測: " << scenario.expected_behavior << std::endl;
            
            // 行動判定
            std::cout << "判定: ";
            if (action_inertia > 0.7) {
                std::cout << "🌟 強く実行 (Very Likely)" << std::endl;
            } else if (action_inertia > 0.5) {
                std::cout << "✅ 実行 (Likely)" << std::endl;
            } else if (action_inertia > 0.3) {
                std::cout << "🤔 検討 (Maybe)" << std::endl;
            } else {
                std::cout << "❌ 回避 (Unlikely)" << std::endl;
            }
            
            std::cout << "理由: " << reasoning << std::endl;
        } else {
            std::cout << "評価失敗: " << ssd_get_last_error_message(engine) << std::endl;
        }
    }
    
    ssd_universal_destroy(engine);
    return 0;
}

int test_personality_comparison() {
    print_test_header("Personality Comparison Test");
    
    SSDUniversalEngine* engine = ssd_universal_create(nullptr);
    if (!engine) {
        std::cout << "ERROR: Failed to create engine" << std::endl;
        return 1;
    }
    
    // 異なる性格の住民比較
    struct NPCProfile {
        const char* name;
        const char* personality;
        double basal_drives[6];
        double routine_strengths[4];
    };
    
    NPCProfile npcs[] = {
        {
            "アクティブな住民「ハート」",
            "元気・外向的",
            {0.2, 0.95, 0.9, 0.6, 0.8, 0.7}, // 安心低、社交高、探索高、創造中、承認高、自立高
            {0.6, 0.95, 0.3, 0.8}  // 朝ルーチン中、挨拶高、読書低、運動高
        },
        {
            "内向的な住民「シルビア」",
            "大人しい・慎重",
            {0.8, 0.4, 0.2, 0.8, 0.3, 0.5}, // 安心高、社交低、探索低、創造高、承認低、自立中
            {0.9, 0.5, 0.9, 0.3}  // 朝ルーチン高、挨拶中、読書高、運動低
        },
        {
            "クリエイティブな住民「ピコ」",
            "芸術家・マイペース",
            {0.5, 0.6, 0.7, 0.95, 0.9, 0.8}, // 安心中、社交中、探索高、創造最高、承認高、自立高
            {0.4, 0.7, 0.6, 0.9}  // 朝ルーチン低、挨拶中、読書中、創作高
        }
    };
    
    // 同じ状況での反応比較
    double common_episodic[3] = {0.5, 0.3, 0.6}; // 普通の記憶
    double afternoon_env[4] = {0.8, 0.5, 0.7, 0.8}; // 晴れた午後
    
    std::cout << "同じ状況（プレイヤーとの挨拶）での性格別反応" << std::endl;
    std::cout << "状況：晴れた午後、プレイヤーが近づいてくる" << std::endl;
    std::cout << "=============================================" << std::endl;
    
    for (const auto& npc : npcs) {
        std::cout << "\n【" << npc.name << "】 - " << npc.personality << std::endl;
        
        double action_inertia, confidence;
        char reasoning[512];
        
        SSDReturnCode result = ssd_evaluate_npc_action(
            engine,
            "greet_player",
            npc.name,
            const_cast<double*>(npc.basal_drives), 6,
            const_cast<double*>(npc.routine_strengths), 4,
            common_episodic, 3,
            afternoon_env, 4,
            &action_inertia,
            &confidence,
            reasoning, sizeof(reasoning)
        );
        
        if (result == SSD_SUCCESS || result == SSD_WARNING_LOW_CONFIDENCE) {
            std::cout << "挨拶慣性: " << std::fixed << std::setprecision(3) << action_inertia;
            
            if (action_inertia > 0.7) {
                std::cout << " → 積極的に挨拶 🌟" << std::endl;
            } else if (action_inertia > 0.5) {
                std::cout << " → 普通に挨拶 ✅" << std::endl;
            } else if (action_inertia > 0.3) {
                std::cout << " → 控えめに挨拶 🤔" << std::endl;
            } else {
                std::cout << " → 挨拶を躊躇 😅" << std::endl;
            }
            
            std::cout << "特徴: " << reasoning << std::endl;
        }
    }
    
    ssd_universal_destroy(engine);
    return 0;
}

int test_comprehensive_inertia_breakdown() {
    print_test_header("Comprehensive Inertia Breakdown Test");
    
    SSDUniversalEngine* engine = ssd_universal_create(nullptr);
    if (!engine) {
        std::cout << "ERROR: Failed to create engine" << std::endl;
        return 1;
    }
    
    // 各構造層の詳細分析用コンポーネント
    
    // 基層コンポーネント（本能・衝動）
    SSDInertiaComponent basal_components[3];
    memset(basal_components, 0, sizeof(basal_components));
    
    strcpy(basal_components[0].component_id, "social_instinct");
    basal_components[0].base_strength = 0.8;
    basal_components[0].usage_frequency = 0.9;
    basal_components[0].success_rate = 0.85;
    basal_components[0].temporal_stability = 0.95;
    basal_components[0].reinforcement_count = 100;
    basal_components[0].decay_resistance = 0.98;
    
    strcpy(basal_components[1].component_id, "comfort_seeking");
    basal_components[1].base_strength = 0.6;
    basal_components[1].usage_frequency = 0.7;
    basal_components[1].success_rate = 0.9;
    basal_components[1].temporal_stability = 0.9;
    basal_components[1].reinforcement_count = 80;
    basal_components[1].decay_resistance = 0.95;
    
    strcpy(basal_components[2].component_id, "exploration_drive");
    basal_components[2].base_strength = 0.4;
    basal_components[2].usage_frequency = 0.3;
    basal_components[2].success_rate = 0.6;
    basal_components[2].temporal_stability = 0.7;
    basal_components[2].reinforcement_count = 30;
    basal_components[2].decay_resistance = 0.8;
    
    // 中核コンポーネント（習慣・ルール）
    SSDInertiaComponent core_components[2];
    memset(core_components, 0, sizeof(core_components));
    
    strcpy(core_components[0].component_id, "greeting_routine");
    core_components[0].base_strength = 0.9;
    core_components[0].usage_frequency = 0.8;
    core_components[0].success_rate = 0.95;
    core_components[0].temporal_stability = 0.85;
    core_components[0].reinforcement_count = 200;
    core_components[0].decay_resistance = 0.9;
    
    strcpy(core_components[1].component_id, "daily_schedule");
    core_components[1].base_strength = 0.7;
    core_components[1].usage_frequency = 0.9;
    core_components[1].success_rate = 0.8;
    core_components[1].temporal_stability = 0.8;
    core_components[1].reinforcement_count = 150;
    core_components[1].decay_resistance = 0.85;
    
    // 上層コンポーネント（価値観・物語）
    SSDInertiaComponent upper_components[2];
    memset(upper_components, 0, sizeof(upper_components));
    
    strcpy(upper_components[0].component_id, "friendship_value");
    upper_components[0].base_strength = 0.8;
    upper_components[0].usage_frequency = 0.6;
    upper_components[0].success_rate = 0.7;
    upper_components[0].temporal_stability = 0.6;
    upper_components[0].reinforcement_count = 50;
    upper_components[0].decay_resistance = 0.7;
    
    strcpy(upper_components[1].component_id, "community_identity");
    upper_components[1].base_strength = 0.6;
    upper_components[1].usage_frequency = 0.4;
    upper_components[1].success_rate = 0.8;
    upper_components[1].temporal_stability = 0.7;
    upper_components[1].reinforcement_count = 40;
    upper_components[1].decay_resistance = 0.75;
    
    // 評価コンテキスト
    SSDEvaluationContext context;
    memset(&context, 0, sizeof(context));
    strcpy(context.context_id, "afternoon_social_interaction");
    context.domain = SSD_DOMAIN_AI;
    context.scale_level = SSD_SCALE_ORGANISM;
    context.time_scale = 1.0;
    context.space_scale = 1.0;
    context.measurement_precision = 0.9;
    context.env_factor_count = 3;
    context.environmental_factors[0] = 0.8; // 天気
    context.environmental_factors[1] = 0.7; // 活動レベル
    context.environmental_factors[2] = 0.9; // 安全性
    
    // 包括的慣性計算
    double total_inertia;
    double layer_breakdown[4];
    char explanation[512];
    
    SSDReturnCode result = ssd_calculate_comprehensive_inertia(
        engine,
        nullptr, 0,           // physical
        basal_components, 3,  // basal
        core_components, 2,   // core  
        upper_components, 2,  // upper
        &context,
        &total_inertia,
        layer_breakdown,
        explanation, sizeof(explanation)
    );
    
    print_result(result, "Comprehensive inertia calculation");
    
    if (result == SSD_SUCCESS || result == SSD_WARNING_LOW_CONFIDENCE) {
        std::cout << "\n=== 構造層別慣性分析 ===" << std::endl;
        std::cout << "総合慣性値: " << std::fixed << std::setprecision(3) << total_inertia << std::endl;
        std::cout << "\n層別内訳:" << std::endl;
        std::cout << "  物理層: " << layer_breakdown[0] << " (物理的制約)" << std::endl;
        std::cout << "  基層層: " << layer_breakdown[1] << " (本能・衝動)" << std::endl;
        std::cout << "  中核層: " << layer_breakdown[2] << " (習慣・ルール)" << std::endl;
        std::cout << "  上層層: " << layer_breakdown[3] << " (価値観・物語)" << std::endl;
        
        std::cout << "\n=== 分析説明 ===" << std::endl;
        std::cout << explanation << std::endl;
        
        // 層別の影響度分析
        std::cout << "\n=== 行動決定への影響度 ===" << std::endl;
        double total_weighted = layer_breakdown[0] + layer_breakdown[1] + layer_breakdown[2] + layer_breakdown[3];
        if (total_weighted > 0) {
            std::cout << "基層影響: " << std::fixed << std::setprecision(1) 
                      << (layer_breakdown[1] / total_weighted * 100) << "% (本能的反応)" << std::endl;
            std::cout << "中核影響: " << (layer_breakdown[2] / total_weighted * 100) << "% (習慣的反応)" << std::endl;
            std::cout << "上層影響: " << (layer_breakdown[3] / total_weighted * 100) << "% (価値観的反応)" << std::endl;
        }
        
        // 行動予測
        std::cout << "\n=== 行動予測 ===" << std::endl;
        if (layer_breakdown[1] > layer_breakdown[2] && layer_breakdown[1] > layer_breakdown[3]) {
            std::cout << "💡 本能主導型: 直感的で自然な反応が予想される" << std::endl;
        } else if (layer_breakdown[2] > layer_breakdown[3]) {
            std::cout << "🔄 習慣主導型: パターン化された安定した反応が予想される" << std::endl;
        } else {
            std::cout << "🎭 価値観主導型: 理念に基づいた考慮深い反応が予想される" << std::endl;
        }
    }
    
    ssd_universal_destroy(engine);
    return 0;
}

int main() {
    std::cout << "SSD Universal Engine - NPC Behavior Test Suite" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // テスト実行
    total_tests++;
    if (test_villager_daily_routine() == 0) passed_tests++;
    
    total_tests++;
    if (test_personality_comparison() == 0) passed_tests++;
    
    total_tests++;
    if (test_comprehensive_inertia_breakdown() == 0) passed_tests++;
    
    // 結果サマリー
    std::cout << "\n===============================================" << std::endl;
    std::cout << "NPC Test Results: " << passed_tests << "/" << total_tests << " passed" << std::endl;
    
    if (passed_tests == total_tests) {
        std::cout << "All NPC tests PASSED! 🎮✅" << std::endl;
        return 0;
    } else {
        std::cout << "Some NPC tests FAILED! 🎮❌" << std::endl;
        return 1;
    }
}