//
// Created by fish on 2025/2/17.
//

#include "ctrl_low_pass_filter.h"

using namespace Controller;

float LowPassFilter::update(const MotorController *motor, float target) {
    return static_cast <float> (filter_.update(target, dt_));
}

void LowPassFilter::clear() {
    filter_.reset(0);
}

