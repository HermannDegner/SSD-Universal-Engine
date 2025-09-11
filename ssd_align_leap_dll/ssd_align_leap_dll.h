
#ifndef SSD_ALIGN_LEAP_DLL_H
#define SSD_ALIGN_LEAP_DLL_H

#ifdef _WIN32
  #ifdef SSD_ALIGN_LEAP_DLL_EXPORTS
    #define SSD_API __declspec(dllexport)
  #else
    #define SSD_API __declspec(dllimport)
  #endif
#else
  #define SSD_API __attribute__((visibility("default")))
#endif

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /* Alignment (deterministic) */
  double G0;        /* base conductance */
  double g;         /* kappa gain */
  double eps_noise; /* align noise (optional) */
  double eta;       /* learning rate */
  double rho;       /* overdrive penalty */
  double lam;       /* forgetting */
  double kappa_min; /* lower bound */

  /* Heat */
  double alpha;     /* accumulation */
  double beta_E;    /* natural decay */

  /* Threshold / jump */
  double Theta0;
  double a1;
  double a2;
  double h0;
  double gamma;

  /* Temperature */
  double T0;
  double c1;
  double c2;

  /* Policy */
  double sigma;       /* noise stddev */

  /* Rewire */
  double delta_w;
  double delta_kappa;
  double c0_cool;     /* cooling factor (0..1) */
  double q_relax;     /* top q% to relax (0..1) */
  double eps_relax;

  /* Epsilon-random */
  double eps0;
  double d1;
  double d2;

  /* Action (unused but reserved) */
  double b_path;
} SSDParams;

typedef struct {
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
} SSDTelemetry;

/* Opaque handle */
typedef struct SSDHandle SSDHandle;

/* Create / destroy */
SSD_API SSDHandle* ssd_create(int32_t N, const SSDParams* params, uint64_t seed);
SSD_API void       ssd_destroy(SSDHandle* h);

/* Step the model by dt with meaning-pressure p. Telemetry is written to *out. */
SSD_API void ssd_step(SSDHandle* h, double p, double dt, SSDTelemetry* out);

/* Utilities */
SSD_API void ssd_get_params(SSDHandle* h, SSDParams* out);
SSD_API void ssd_set_params(SSDHandle* h, const SSDParams* in);
SSD_API int32_t ssd_get_N(SSDHandle* h);

/* Optional: read a row of kappa (length must be >= N). Returns count written. */
SSD_API int32_t ssd_get_kappa_row(SSDHandle* h, int32_t row, double* out_buf, int32_t len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SSD_ALIGN_LEAP_DLL_H */
