//
// Created by fish on 2024/11/17.
//

#include "app_gimbal.h"

#include <cstring>

#include "alg_crc.h"
#include "alg_filter.h"
#include "app_gimbal_shooter.h"
#include "app_ins.h"
#include "app_motor.h"
#include "app_msg.h"
#include "app_sys.h"
#include "bsp_rc.h"
#include "bsp_time.h"
#include "ctrl_low_pass_filter.h"
#include "ctrl_pid.h"
#include "dev_motor_dji.h"
#include "sys_task.h"

#ifdef COMPILE_GIMBAL

using namespace Motor;
using namespace Controller;

/* 摩擦轮 */

/* Yaw & Pitch */
MotorController yaw(std::make_unique <DJIMotor> (
    "yaw", DJIMotor::GM6020, (DJIMotor::Param) { 0x01, E_CAN2, DJIMotor::VOLTAGE }
));
MotorController pit(std::make_unique <DJIMotor> (
    "pit", DJIMotor::M3508, (DJIMotor::Param) { 0x03, E_CAN2, DJIMotor::CURRENT }
));

static float calc_delta(float full, float current, float target) {
    float dt = target - current;
    if(2 * dt >  full) dt -= full;
    if(2 * dt < -full) dt += full;
    return dt;
}

const auto rc = bsp_rc_data();
const auto ins = app_ins_data();

float target = 0, yaw_lst_angle = 0, yaw_sum_angle = 0, yaw_target = 0;
void set_target(bsp_uart_e e, uint8_t *s, uint16_t l) {
    sscanf((char *) s, "%f", &target);
}

void send_msg_to_chassis() {
    app_msg_gimbal_to_chassis data = {
        .yaw_motor_angle = yaw.device()->angle,
        .rc_reserved = rc->reserved,
        .rc_active = bsp_time_get_ms() - rc->timestamp < 100
        // .rc_active = false
    };
    memcpy(data.rc_l, rc->rc_l, sizeof(rc->rc_l));
    bsp_uart_send(E_UART_485, reinterpret_cast <uint8_t *> (&data), sizeof(data));
}

// 静态任务，在 CubeMX 中配置
void app_gimbal_task(void *args) {
    // Wait for system init.
    while(!app_sys_ready()) OS::Task::SleepMilliseconds(10);

    bsp_uart_set_callback(E_UART_DEBUG, set_target);

    while(true) {
#ifdef USE_DUAL_CONTROLLER
        send_msg_to_chassis();
#endif
        yaw_sum_angle += calc_delta(360, yaw_lst_angle, ins->yaw);
        yaw_lst_angle = ins->yaw;

        if(bsp_time_get_ms() - rc->timestamp > 100) {
            pit.clear();
            pit.update(0);
            yaw.clear();
            yaw.update(yaw_target);
            app_gimbal_shooter_update(0, 0, 0);
            OS::Task::SleepMilliseconds(1);
            continue;
        }
        if(rc->s_l == 1) {
            app_gimbal_shooter_update(5600, 5100, 500);
        } else if (rc->s_l == 0){
            app_gimbal_shooter_update(5600, 5100, 0);
        } else {
            app_gimbal_shooter_update(0, 0, 0);
        }

        if(rc->s_r == 0) {
            pit.update(rc->rc_r[1] / 660.0 * 3000.0);
        } else {
            pit.update(static_cast <float> (rc->s_r) * 10);
        }

        yaw_target -= static_cast <float> (rc->rc_r[0]) / 660.0f * 0.18f;
        yaw.update(yaw_target);

        // app_msg_vofa_send(E_UART_DEBUG, {
        //     yaw_target,
        //     yaw_sum_angle,
        //     yaw.device()->current,
        // });
        OS::Task::SleepMilliseconds(1);
    }
}

void app_gimbal_init() {
    /* 摩擦轮 */
    app_gimbal_shooter_init();
    /* Yaw & Pitch */
    yaw.init(); pit.init();
    pit.add_controller(
        [](const auto x) -> double { return x->device()->speed; },
        std::make_unique <PID> (7.5, 0.5, 0.0, 7500, 1000)
    );
    pit.use_stall_detect = true;
    pit.stall_detector_current_threshold = 5000;
    pit.stall_detector_time_threshold = 250;

    yaw.add_controller(
        [](const auto x) -> double { return yaw_sum_angle; },
        std::make_unique <PID> (19, 0, 0, 360, 0)
    );
    yaw.add_controller(
        [](const auto x) -> double { return ins->raw.gyro[2] / M_PI * 180; },
        std::make_unique <PID> (425, 2.5, 0.0, 25000, 20000)
    );
    yaw.add_controller(
        std::make_unique <LowPassFilter> (50, 0.001)
    );
}

#endif