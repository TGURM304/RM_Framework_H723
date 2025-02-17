//
// Created by fish on 2024/11/17.
//

#include "app_gimbal.h"

#include "app_motor.h"
#include "app_msg.h"
#include "app_sys.h"
#include "bsp_rc.h"
#include "ctrl_pid.h"
#include "dev_motor_dji.h"
#include "sys_task.h"

#ifdef COMPILE_GIMBAL

using namespace Motor;

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


const auto rc = bsp_rc_data();

// 静态任务，在 CubeMX 中配置
void app_gimbal_task(void *args) {
    // Wait for system init.
    while(!app_sys_ready()) OS::Task::SleepMilliseconds(10);

    float speed_1 = 0, speed_2 = 0;

    while(true) {
        speed_1 = static_cast <float> (rc->s_l) * 5000;
        speed_2 = static_cast <float> (rc->s_r) * 5000;

        m_1_l.update( speed_1);
        m_1_m.update( speed_1);
        m_1_r.update(-speed_1);

        m_2_l.update( speed_2);
        m_2_r.update(-speed_2);

        app_msg_vofa_send(E_UART_DEBUG, {
            speed_1,
            m_1_l.speed,
            m_1_m.speed,
            m_1_r.speed,
            speed_2,
            m_2_l.speed,
            m_2_r.speed
        });
        OS::Task::SleepMilliseconds(1);
    }
}

void app_gimbal_init() {
    m_1_l.init();
    m_1_l.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <Controller::PID> (16.5, 1.0, 0.0, 16384, 1000)
    );
    m_1_m.init();
    m_1_m.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <Controller::PID> (16.5, 1.0, 0.0, 16384, 1000)
    );
    m_1_r.init();
    m_1_r.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <Controller::PID> (16.5, 1.0, 0.0, 16384, 1000)
    );
    m_2_l.init();
    m_2_l.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <Controller::PID> (16.5, 1.0, 0.0, 16384, 1000)
    );
    m_2_r.init();
    m_2_r.add_controller(
        [](auto x) -> double { return x->device()->speed; },
        std::make_unique <Controller::PID> (16.5, 1.0, 0.0, 16384, 1000)
    );
}

#endif