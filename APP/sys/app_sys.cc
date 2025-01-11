//
// Created by fish on 2024/11/3.
//

#include "app_sys.h"

#include "app_ins.h"
#include "app_motor.h"
#include "bsp_uart.h"
#include "bsp_can.h"
#include "bsp_led.h"
#include "bsp_adc.h"
#include "bsp_rc.h"
#include "dev_motor.h"
#include "sys_task.h"
#include "app_chassis.h"
#include "app_gimbal.h"
#include "app_conf.h"
#include "bsp_buzzer.h"

#include <cstdio>
#include <cmath>

#include "bsp_def.h"

#include "app_msg.h"

bool inited_ = false;

bool app_sys_ready() {
    return inited_ && app_ins_status() == 2;
}

int8_t r = 0, g = 0, b = 0;

void app_sys_init() {
    app_ins_init();

#if defined(COMPILE_CHASSIS)
    app_chassis_init();
#endif
#ifdef COMPILE_GIMBAL
    app_gimbal_init();
#endif

    inited_ = true;
}

// 放一些系统级任务
void app_sys_task() {
    bsp_buzzer_flash(1976, 0.5, 250);
    bsp_led_set(0, 0, 255);
    app_sys_init();
    bsp_led_set(0, 255, 0);
    while(app_ins_status() != 2) OS::Task::SleepMilliseconds(1);
    bsp_buzzer_flash(1976, 0.5, 125);
    OS::Task::SleepMilliseconds(50);
    bsp_buzzer_flash(1976, 0.5, 125);
    while(true) {
        bsp_led_set(std::abs(r), std::abs(g), std::abs(b));
        if(++ r > 50) r = -50;
        if(++ g > 50) g = -50;
        if(++ b > 50) b = -50;
        OS::Task::SleepMilliseconds(10);
    }
}

/*
 *  Note:
 *  - 为了确保代码同时适用于单板、双板控制场景，这里同时开两个任务，但不必同时实现。
 *  - 若 chassis / gimbal 任务未实现，则进入下面的函数删除任务。
 *  Warning:
 *  - 若不理解下面的代码是什么意思，请不要随意修改。
 */

__weak void app_chassis_task(void *argument) {
    OS::Task::Current().Delete();
}
__weak void app_gimbal_task(void *argument) {
    OS::Task::Current().Delete();
}
__weak void dev_dji_motor_task(void *argument) {
    OS::Task::Current().Delete();
}
__weak void app_ins_task(void *argument) {
    OS::Task::Current().Delete();
}