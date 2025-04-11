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
#include "motor_base.h"
#include "sys_task.h"
#include "app_chassis.h"
#include "app_gimbal.h"
#include "app_conf.h"
#include "bsp_buzzer.h"

#include <cstdio>
#include <cmath>
#include <cstring>

#include "bsp_def.h"

#include "app_msg.h"
#include "app_referee.h"
#include "app_sys_err.h"
#include "app_music.h"
#include "app_terminal.h"
#include "bsp_flash.h"
#include "usb_device.h"

bool inited_ = false;

bool app_sys_ready() {
    return inited_ && app_ins_status() == 2 && bsp_usb_inited();
}

static app_sys_conf_t config;
static app_sys_flash_t flash;

const app_sys_conf_t *app_sys_conf() {
    return &config;
}

void app_sys_terminal_init() {
    app_terminal_register_cmd("sys", "system commands", [](const auto &args) -> bool {
        if(args.size() == 1) {
            TERMINAL_INFO("usage: sys vbus");
            return true;
        }
        if(args[1] == "vbus") {
            auto running = app_terminal_running_flag();
            while(*running) {
                TERMINAL_INFO("vbus: %f\r\n", bsp_adc_vbus());
                OS::Task::SleepMilliseconds(10);
            }
        }
        return true;
    });
}

void app_sys_init() {
    app_ins_init();
#ifdef USE_TERMINAL
    app_terminal_init();
#endif
#ifdef COMPILE_CHASSIS
    config.type |= 0b01;
    app_chassis_init();
#endif
#ifdef COMPILE_GIMBAL
    config.type |= 0b10;
    app_gimbal_init();
#endif
#ifdef USE_REFEREE_SYSTEM
    app_referee_init();
#endif
#ifdef USE_FLASH_CHECK
    // 校验 flash 中的 brief，若此处校验不通过，请连接 terminal 执行 flash clear
    bsp_flash_read("sys", &flash, sizeof(flash));
    if(flash.flag == SYS_FLASH_KEY) {
        if(strcmp(config.brief, flash.brief) != 0 or (config.type and flash.type != config.type)) {
            app_sys_err_mark(SYS_ERR_FLASH_WRONG_BRIEF);
        }
    } else {
        flash.flag = SYS_FLASH_KEY;
        strcpy(flash.brief, config.brief);
        flash.type = config.type;
        bsp_flash_write("sys", &flash, sizeof(flash));
    }
#endif
    inited_ = true;
}

// 放一些系统级任务
void app_sys_task() {
    bsp_buzzer_flash(1976, 0.5, 250);
    bsp_led_set(0, 0, 255);
    app_sys_init();
    bsp_led_set(0, 255, 0);
    while(app_ins_status() != 2)
        OS::Task::SleepMilliseconds(1);
    if(!app_sys_err()) {
        // bsp_buzzer_flash(1976, 0.5, 125);
        // OS::Task::SleepMilliseconds(50);
        // bsp_buzzer_flash(1976, 0.5, 125);
        app_sys_music_play(E_MUSIC_BOOT);
    }
    int8_t r = 0, g = 0, b = 0;
    while(true) {
        if(!app_sys_err()) {
            // 系统正常工作，白色呼吸灯
            bsp_led_set(std::abs(r), std::abs(g), std::abs(b));
            if(++r > 50) r = -50;
            if(++g > 50) g = -50;
            if(++b > 50) b = -50;
            OS::Task::SleepMilliseconds(10);
        } else {
            // FLASH 描述符错误，黄灯快闪
            if(app_sys_err_check(SYS_ERR_FLASH_WRONG_BRIEF)) bsp_led_set(50, 50, 0);
            OS::Task::SleepMilliseconds(100);
            bsp_led_set(0, 0, 0);
            OS::Task::SleepMilliseconds(100);
        }
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
