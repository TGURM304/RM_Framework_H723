//
// Created by fish on 2025/2/17.
//

#pragma once

#include "alg_filter.h"
#include "controller_base.h"

namespace Controller {
class LowPassFilter : public Base {
public:
    typedef struct {
        double Kp, Ki, Kd, out_limit, iout_limit;
    } pid_param_t;
    LowPassFilter() = default;
    ~LowPassFilter() override = default;
    LowPassFilter(float cut_freq, float dt)
        : filter_(cut_freq), dt_(dt)
    {
        type = "Filter";
    }
    void clear() override;
    float update(float current, float target) override { return 0; }
    float update(const MotorController *motor, float target) override;
private:
    float dt_ = 0;
    Algorithm::LowPassFilter filter_;
};
}
