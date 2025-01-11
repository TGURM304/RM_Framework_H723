//
// Created by fish on 2024/11/16.
//

#include "app_motor_base_controller.h"

#include <cmath>

#include "bsp_def.h"
#include "bsp_time.h"
#include "cmsis_os2.h"
#include "sys_task.h"

#define ABS(x) (((x) < 0) ? -(x) : (x))

static OS::Task motor_task;

// Convert degree to encoder val.
// [0, 360) -> [0, 8192)
static float deg2encoder(float x, float zero) {
    float ret = x * 8192 / 360 + zero;
    if(ret > 8191) ret -= 8192;
    return ret;
}

// Convert encoder val to degree.
// [0, 8192) -> [0, 360)
static float encoder2deg(float x, float zero) {
    x -= zero;
    if(x < 0) x += 8192;
    return x * 360 / 8192;
}

static double encoder_delta(double current, double target) {
    double dt = target - current;
    // 0 - 8191
    if(dt >  4096) dt -= 8192;
    if(dt < -4096) dt += 8192;
    return dt;
}

static double degree_delta(double current, double target) {
    double dt = target - current;
    // 0 - 359
    if(dt >  180) dt -= 360;
    if(dt < -180) dt += 360;
    return dt;
}

MotorBaseController <Motor::DJIMotor> :: MotorBaseController(const char *name, const Motor::DJIMotor::Model &model, const Motor::DJIMotor::Param &param,
        uint8_t control_mode,
        const Algorithm::PID::pid_param_t &pid_speed, const Algorithm::PID::pid_param_t &pid_angle) :
        motor_(name, model, param), control_mode_(control_mode), target(0.0) {

    pid_speed_.set_para(pid_speed);
    pid_angle_.set_para(pid_angle);
}

void MotorBaseController <Motor::DJIMotor> ::init() {
    motor_.init();

    auto fn = [](MotorBaseController *p) -> void {
        while(true) {
            p->task();
            osDelay(1);
        }
    };

    motor_task.Create(fn, this, "motor_task", 128, OS::Task::REALTIME);
}

void MotorBaseController <Motor::DJIMotor> ::task() {
    // Offline
    if(bsp_time_get_ms() - motor_.status.last_online_time > 500) {
        if(!offline_) {
            offline_ = true;
            target = 0;
            motor_.clear();
            pid_angle_.clear();
            pid_speed_.clear();
        }
        return;
    }

    offline_ = false;

    // 把电机数据搬过来
    speed = motor_.status.speed;
    angle = use_degree_angle ? encoder2deg(motor_.status.angle, encoder_zero) : motor_.status.angle;

    // 计算总角度
    if(use_ext_angle) {
        if(use_degree_angle)
            sum_angle -= static_cast <float> (degree_delta(angle, lst_angle));
        else
            sum_angle -= static_cast <float> (encoder_delta(angle, lst_angle));
        lst_angle = angle;
    }

    // Relax
    if(is_relax_) return;

    // 堵转保护，暂时先这样写（不是很合理，后面要把加速度考虑进去）
    if(ABS(motor_.status.current) > 10000) {
        if(++ err_count_ == 1000) {
            relax();
            return;
        }
    } else {
        err_count_ = 0;
    }

    double output = reverse ? -target : target;
    if(control_mode_ & PID_ANGLE) {
        // 建议 reverse 仅在速度环中使用，否则可能导致逻辑混乱
        BSP_ASSERT(!reverse);
        if(use_ext_angle)
            output = pid_angle_.update(sum_angle, output);
        else
            output = pid_angle_.update(0, use_degree_angle ? degree_delta(angle, output) : encoder_delta(angle, output));
    }
    if(control_mode_ & PID_SPEED) {
        output = pid_speed_.update(speed, output);
    }
    motor_.update(static_cast <float> (output));
}