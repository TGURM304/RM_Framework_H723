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

/* 系统是否初始化完成标志 */
bool inited_ = false;

/**
 * @brief 检查系统是否已经准备就绪
 *
 * 系统就绪条件：
 * - 系统初始化完成
 * - IMU 正常就绪
 * - USB 初始化完成
 *
 * @return true  系统已准备就绪
 * @return false 系统未准备好
 */
bool app_sys_ready() {
    return inited_ && app_ins_status() == 2 && bsp_usb_inited();
}

/* 系统配置结构体 */
static app_sys_conf_t config;
/* Flash 配置缓存 */
static app_sys_flash_t flash;

/**
 * @brief 获取系统配置指针
 *
 * @return const app_sys_conf_t* 指向系统配置的指针
 */
const app_sys_conf_t *app_sys_conf() {
    return &config;
}

/**
 * @brief 初始化系统终端命令
 *
 * 注册命令：
 * - sys vbus：循环输出电源总线电压
 */
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

/**
 * @brief 系统初始化
 *
 * - 初始化 IMU、终端、底盘、云台、裁判系统
 * - 校验 Flash 配置并处理错误
 * - 设置系统初始化完成标志
 */
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

/**
 * @brief 系统主任务
 *
 * 功能：
 * - 开机蜂鸣器提示
 * - LED 灯光效果：正常工作白色呼吸灯，Flash 描述符错误黄灯闪烁
 * - 等待 IMU 就绪
 * - 播放开机音乐（建议为春日影）
 */
void app_sys_task() {
    bsp_buzzer_flash(1976, 0.5, 250);
    bsp_led_set(0, 0, 255);
    app_sys_init();
    bsp_led_set(0, 255, 0);
    while(app_ins_status() != 2)
        OS::Task::SleepMilliseconds(1);
    if(!app_sys_err()) {
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

/**
 * @brief 底盘任务
 *
 * 若未实现底盘任务，则直接删除任务。
 * @param argument 任务参数
 */
__weak void app_chassis_task(void *argument) {
    OS::Task::Current().Delete();
}

/**
 * @brief 云台任务函数
 *
 * 若未实现云台任务，则直接删除任务。
 * @param argument 任务参数
 */
__weak void app_gimbal_task(void *argument) {
    OS::Task::Current().Delete();
}

/**
 * @brief DJI 电机任务函数
 *
 * 若未实现 DJI 电机任务，则直接删除任务。
 * @param argument 任务参数
 */
__weak void dev_dji_motor_task(void *argument) {
    OS::Task::Current().Delete();
}

/**
 * @brief IMU任务函数
 *
 * 若未实现 IMU 任务，则直接删除任务。
 * @param argument 任务参数
 */
__weak void app_ins_task(void *argument) {
    OS::Task::Current().Delete();
}
