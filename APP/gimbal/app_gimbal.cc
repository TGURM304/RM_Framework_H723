//
// Created by fish on 2024/11/17.
//

#include "app_gimbal.h"

#include <cstdio>

#include "alg_pid.h"
#include "app_ins.h"
#include "app_msg.h"
#include "sys_task.h"
#include "app_sys.h"
#include "app_vision.h"
#include "ctrl_forward_feed.h"
#include "ctrl_pid.h"
#include "dev_motor_dji.h"

#ifdef COMPILE_GIMBAL

#include "app_motor.h"
#include "dev_motor.h"

using namespace Motor;

auto ins = app_ins_data();

MotorController motor(
    std::make_unique <DJIMotor> (
        "test", DJIMotor::GM6020, (DJIMotor::Param) { .id = 0x01, .port = E_CAN1, .mode = DJIMotor::VOLTAGE }
    )
);

double forward_feed() {
    return motor.status()->angle;
}

// 静态任务，在 CubeMX 中配置
void app_gimbal_task(void *args) {
    // Wait for system init.
    while(!app_sys_ready()) OS::Task::SleepMilliseconds(10);

    motor.add_controller(
        [&]() -> double { return ins->raw.gyro[0] * 180 / M_PI; },
        std::make_unique <Controller::PID> (145, 3, 0, 25000, 20000)
    );
    motor.add_controller(
        [&]() -> double { return ins->yaw; },
        std::make_unique <Controller::PID> (13, 0, 0, 550, 0)
    );
    motor.add_controller(
        forward_feed,
        std::make_unique <Controller::ForwardFeed> ()
    );

    while(true) {
        motor.update(0);
        app_msg_vofa_send(E_UART_DEBUG, {
            ins->roll,
            ins->pitch,
            ins->yaw,
            motor.output
        });
        OS::Task::SleepMilliseconds(1);
    }
}

void app_gimbal_init() {
    motor.init();
}

#endif