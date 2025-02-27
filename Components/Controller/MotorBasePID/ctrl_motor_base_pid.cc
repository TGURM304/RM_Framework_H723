//
// Created by fish on 2025/1/3.
//

#include "ctrl_motor_base_pid.h"

#include "app_motor.h"

using namespace Controller;

static float calc_delta(float full, float current, float target) {
    float dt = target - current;
    if(2 * dt >  full) dt -= full;
    if(2 * dt < -full) dt += full;
    return dt;
}

float MotorBasePID::update(const MotorController *motor, float target) {
    if((mode_ & PID_ANGLE) and angle_ != nullptr)
        target = use_micro_arc_
            ? angle_->update(0.0f, calc_delta(motor->use_degree_angle ? 360 : 8192, motor->angle, target))
            : angle_->update(motor->angle, target);
    if((mode_ & PID_SPEED) and speed_ != nullptr)
        target = speed_->update(motor->speed, target);
    return target;
}

void MotorBasePID::clear() {
    if(speed_ != nullptr) speed_->clear();
    if(angle_ != nullptr) angle_->clear();
}

