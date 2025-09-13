#pragma once
#include <stdint.h>

#ifdef _WIN32
  #ifdef SSD_CORE_EXPORTS
    #define SSD_API __declspec(dllexport)
  #else
    #define SSD_API __declspec(dllimport)
  #endif
#else
  #define SSD_API __attribute__((visibility("default")))
#endif

struct SSDParams {
  // 整合パラメータ
  double G0 = 0.5;
  double g = 0.7;
  double eps_noise = 0.0;
  double eta = 0.3;
  double rho = 0.3;
  double lam = 0.02;
  double kappa_min = 0.0;
  
  // 熱パラメータ
  double alpha = 0.6;
  double beta_E = 0.15;
  
  // 跳躍パラメータ
  double Theta0 = 1.0;
  double a1 = 0.5;
  double a2 = 0.4;
  double h0 = 0.2;
  double gamma = 0.8;
  
  // 温度パラメータ
  double T0 = 0.3;
  double c1 = 0.5;
  double c2 = 0.6;
  double sigma = 0.2;
  
  // 再配線パラメータ
  double delta_w = 0.2;
  double delta_kappa = 0.2;
  double c0_cool = 0.6;
  double q_relax = 0.1;
  double eps_relax = 0.01;
  double eps0 = 0.02;
  double d1 = 0.2;
  double d2 = 0.2;
  double b_path = 0.5;
};

struct SSDTelemetry {
  double E;
  double Theta;
  double h;
  double T;
  double H;
  double J_norm;
  double align_eff;
  double kappa_mean;
  int32_t current;
  int32_t did_jump;
  int32_t rewired_to;
};

struct SSDHandle; // 不透明ハンドル

#ifdef __cplusplus
extern "C" {
#endif

SSD_API SSDHandle* ssd_create(int32_t N, const SSDParams* params, uint64_t seed);
SSD_API void ssd_destroy(SSDHandle* h);
SSD_API void ssd_step(SSDHandle* h, double p, double dt, SSDTelemetry* out);
SSD_API void ssd_get_params(SSDHandle* h, SSDParams* out);
SSD_API void ssd_set_params(SSDHandle* h, const SSDParams* in);
SSD_API int32_t ssd_get_N(SSDHandle* h);
SSD_API int32_t ssd_get_kappa_row(SSDHandle* h, int32_t row, double* out_buf, int32_t len);

#ifdef __cplusplus
}
#endif