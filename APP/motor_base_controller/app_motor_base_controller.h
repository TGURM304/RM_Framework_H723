//
// Created by fish on 2024/11/16.
//

#pragma once
#include "alg_pid.h"
#include "dev_motor_dji.h"

#define MOTOR_CONTROLLER_LIMIT 16

typedef enum {
    PID_NONE  = 0b00,
    PID_SPEED = 0b01,
    PID_ANGLE = 0b10
} app_motor_ctrl_e;

template <typename T>
class MotorBaseController {
public:
    MotorBaseController() = default;
};

template <>
class MotorBaseController <Motor::DJIMotor> {
public:
    MotorBaseController(const char *name, const Motor::DJIMotor::Model &model, const Motor::DJIMotor::Param &param,
        uint8_t control_mode,
        const Algorithm::PID::pid_param_t &pid_speed, const Algorithm::PID::pid_param_t &pid_angle);

    void init();
    void task();
    void relax() { is_relax_ = true, pid_speed_.clear(), pid_angle_.clear(), motor_.update(0); };
    void activate() { if(is_relax_) is_relax_ = false; };

    float target, speed = 0, angle = 0, lst_angle = 0, sum_angle = 0, encoder_zero = 0;
    bool reverse = false;               // reverse，启用后会翻转电机速度正方向（仅适用于单速度环控制模式）
    bool use_ext_angle = false;         // Extra angle，启用后会计算总角度，且依靠总角度闭环
    bool use_degree_angle = false;      // Degree angle，启用后会以 encoder_zero 为零点将电机角度映射到 [0, 360) (deg) 范围，逆时针为正方向（若电反装则为顺时针）
    float extra_output = 0;
    Motor::DJIMotor *device() { return &motor_; };

private:
    uint16_t err_count_ = 0;
    bool is_relax_ = false, offline_ = false;
    uint8_t control_mode_;
    Motor::DJIMotor motor_;
    Algorithm::PID pid_speed_, pid_angle_;
};