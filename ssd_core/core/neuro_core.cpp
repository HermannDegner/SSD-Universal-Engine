#include "neuro_core.h"
#include <algorithm>

static inline float clamp01(float v) {
    return v < 0 ? 0 : (v > 1 ? 1 : v);
}

void NeuroCore::Tick(float dt) {
    auto step = [&](float& v, float b, float tau) {
        if (tau <= 1e-3f) return;
        v += (b - v) * (dt / tau);
        v = clamp01(v);
        };

    step(x.DA, baseline.DA, tau_DA);
    step(x.S5, baseline.S5, tau_S5);
    step(x.NE, baseline.NE, tau_NE);
    step(x.AD, baseline.AD, tau_AD);
    step(x.END, baseline.END, tau_END);
    step(x.OXT, baseline.OXT, tau_OXT);
    step(x.CORT, baseline.CORT, tau_CORT);
}

// 代表的イベント定義
struct NeuroEvent {
    const char* id;
    NeuroState delta;
};

static const NeuroEvent kEvents[] = {
    {"praise",           {+0.10f, +0.05f,   0.0f,   0.0f,   0.0f, +0.10f, -0.02f}},
    {"insult_god",       {-0.05f, -0.10f, +0.12f, +0.12f,   0.0f, -0.08f, +0.15f}},
    {"ritual_success",   {+0.08f, +0.05f,   0.0f,   0.0f,   0.0f, +0.10f, -0.02f}},
    {"taboo_violation",  {-0.05f, -0.05f, +0.10f, +0.10f,   0.0f, -0.05f, +0.12f}},
    {"comfort",          {+0.02f, +0.05f, -0.05f,   0.0f, +0.05f, +0.08f, -0.05f}},
};

void NeuroCore::ApplyEvent(std::string_view id) {
    auto add_clamp = [](float& v, float delta) {
        v = clamp01(v + delta);
        };

    for (const auto& event : kEvents) {
        if (id == event.id) {
            add_clamp(x.DA, event.delta.DA);
            add_clamp(x.S5, event.delta.S5);
            add_clamp(x.NE, event.delta.NE);
            add_clamp(x.AD, event.delta.AD);
            add_clamp(x.END, event.delta.END);
            add_clamp(x.OXT, event.delta.OXT);
            add_clamp(x.CORT, event.delta.CORT);
            return;
        }
    }
}