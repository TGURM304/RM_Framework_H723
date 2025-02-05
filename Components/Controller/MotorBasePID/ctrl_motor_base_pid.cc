//
// Created by fish on 2025/1/3.
//

#include "ctrl_motor_base_pid.h"

#include "app_motor.h"

using namespace Controller;

float MotorBasePID::update(const MotorController *motor, float target) {
    if((mode_ & PID_ANGLE) and angle_ != nullptr)
        target = angle_->update(motor->angle, target);
    if((mode_ & PID_SPEED) and speed_ != nullptr)
        target = speed_->update(motor->speed, target);
    return target;
}

void MotorBasePID::clear() const {
    speed_->clear(); angle_->clear();
}

