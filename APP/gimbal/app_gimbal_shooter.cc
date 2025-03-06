//
// Created by fish on 2025/2/19.
//

#include "app_gimbal_shooter.h"

#include <memory>

#include "app_motor.h"
#include "app_msg.h"
#include "bsp_rc.h"
#include "ctrl_pid.h"
#include "dev_motor_dji.h"
#include "app_conf.h"

#ifdef COMPILE_GIMBAL

using namespace Motor;
using namespace Controller;

// 第一级 左 正向
MotorController m_1_l(std::make_unique <DJIMotor> (
    "m_1_l", DJIMotor::M3508, (DJIMotor::Param) { 0x01, E_CAN1, DJIMotor::CURRENT }
));

// 第一级 中 正向
MotorController m_1_m(std::make_unique <DJIMotor> (
    "m_1_m", DJIMotor::M3508, (DJIMotor::Param) { 0x02, E_CAN1, DJIMotor::CURRENT }
));

// 第一级 右 反向
MotorController m_1_r(std::make_unique <DJIMotor> (
    "m_1_r", DJIMotor::M3508, (DJIMotor::Param) { 0x03, E_CAN1, DJIMotor::CURRENT }
));

// 第二级 左 正向
MotorController m_2_l(std::make_unique <DJIMotor> (
    "m_2_l", DJIMotor::M3508, (DJIMotor::Param) { 0x04, E_CAN1, DJIMotor::CURRENT }
));

// 第二级 右 反向
MotorController m_2_r(std::make_unique <DJIMotor> (
    "m_2_r", DJIMotor::M3508, (DJIMotor::Param) { 0x05, E_CAN1, DJIMotor::CURRENT }
));

// 拨弹盘 1
MotorController t_l(std::make_unique <DJIMotor> (
    "t_l", DJIMotor::M3508, (DJIMotor::Param) { 0x01, E_CAN2, DJIMotor::CURRENT }
));

// 拨弹盘 2
MotorController t_r(std::make_unique <DJIMotor> (
    "t_r", DJIMotor::M3508, (DJIMotor::Param) { 0x02, E_CAN2, DJIMotor::CURRENT }
));

const auto rc = bsp_rc_data();

void app_gimbal_shooter_update(float speed_1, float speed_2, float speed_trigger) {
    m_1_l.update( speed_1);
    m_1_m.update( speed_1);
    m_1_r.update(-speed_1);

    m_2_l.update( speed_2);
    m_2_r.update(-speed_2);

    t_l.update(speed_trigger);
    t_r.update(speed_trigger);

    // app_msg_vofa_send(E_UART_DEBUG, {
    //     std::abs(m_1_l.speed),
    //     std::abs(m_1_m.speed),
    //     std::abs(m_1_r.speed),
    //     std::max(std::abs(m_1_l.speed), std::max(std::abs(m_1_m.speed), std::abs(m_1_r.speed))) - std::min(std::abs(m_1_l.speed), std::min(std::abs(m_1_m.speed), std::abs(m_1_r.speed))),
    //     std::abs(m_2_l.speed),
    //     std::abs(m_2_r.speed),
    //     std::abs(std::abs(m_2_l.speed) - std::abs(m_2_r.speed))
    // });
}

void app_gimbal_shooter_init() {
    m_1_l.init();
    m_1_l.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <PID> (16.5, 1.0, 0.0, 16384, 1000)
    );
    m_1_m.init();
    m_1_m.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <PID> (16.5, 1.0, 0.0, 16384, 1000)
    );
    m_1_r.init();
    m_1_r.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <PID> (16.5, 1.0, 0.0, 16384, 1000)
    );
    m_2_l.init();
    m_2_l.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <PID> (16.5, 1.0, 0.0, 16384, 1000)
    );
    m_2_r.init();
    m_2_r.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <PID> (16.5, 1.0, 0.0, 16384, 1000)
    );
    t_l.init();
    t_l.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <PID> (10.0, 1.0, 0.0, 16384, 10000)
    );
    t_l.use_stall_detect = true; t_l.stall_detector_time_threshold = 350; t_l.stall_detector_current_threshold = 13800;
    t_r.init();
    t_r.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <PID> (10.0, 1.0, 0.0, 16384, 10000)
    );
    t_r.use_stall_detect = true; t_r.stall_detector_time_threshold = 350; t_r.stall_detector_current_threshold = 13800;
}

#endif