#pragma once
#include <string_view>

struct NeuroState { 
  float DA = 0.5f;   // ドーパミン
  float S5 = 0.5f;   // セロトニン  
  float NE = 0.5f;   // ノルアドレナリン
  float AD = 0.5f;   // アドレナリン
  float END = 0.5f;  // エンドルフィン
  float OXT = 0.5f;  // オキシトシン
  float CORT = 0.5f; // コルチゾル
};

struct NeuroCore {
  NeuroState baseline{};
  NeuroState x{};
  
  // 時定数（秒）
  float tau_DA = 30.0f;
  float tau_S5 = 45.0f;
  float tau_NE = 20.0f;
  float tau_AD = 8.0f;
  float tau_END = 40.0f;
  float tau_OXT = 35.0f;
  float tau_CORT = 120.0f;

  void Tick(float dt_sec);
  void ApplyEvent(std::string_view event_id);
  const NeuroState& Get() const { return x; }
  
  // オキシトシンブースト計算
  float OxtBoost(float gain = 0.3f) const { 
    return 1.0f + gain * (2.0f * x.OXT - 1.0f); 
  }
};