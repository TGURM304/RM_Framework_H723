//
// Created by fish on 2024/11/17.
//

#include "app_gimbal.h"

#include "app_gimbal_shooter.h"
#include "app_motor.h"
#include "app_msg.h"
#include "app_sys.h"
#include "bsp_rc.h"
#include "bsp_time.h"
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

const auto rc = bsp_rc_data();

float target = 0;
void set_target(bsp_uart_e e, uint8_t *s, uint16_t l) {
    sscanf((char *) s, "%f", &target);
}

// 静态任务，在 CubeMX 中配置
void app_gimbal_task(void *args) {
    // Wait for system init.
    while(!app_sys_ready()) OS::Task::SleepMilliseconds(10);

    bsp_uart_set_callback(E_UART_DEBUG, set_target);

    while(true) {
        if(bsp_time_get_ms() - rc->timestamp > 100) {
            pit.clear();
            pit.update(0);
            app_gimbal_shooter_update(0, 0, 0);
            OS::Task::SleepMilliseconds(1);
            continue;
        }

        if(rc->s_l == 1) {
            // TODO: 调整摩擦轮速度（第一级，第二级）
            app_gimbal_shooter_update(5000, 5100, 500);
        } else {
            app_gimbal_shooter_update(0, 0, 0);
        }

        if(rc->s_r == 0) {
            pit.update(rc->rc_r[1] / 660.0 * 3000.0);
        } else {
            pit.update(static_cast <float> (rc->s_r) * 10);
        }

        // app_msg_vofa_send(E_UART_DEBUG, {
        //     pit.device()->speed,
        //     pit.output,
        //     pit.current
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
}

#endif