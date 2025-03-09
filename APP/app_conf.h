//
// Created by fish on 2024/11/16.
//

#ifndef APP_CONF_H
#define APP_CONF_H

/*
 *  app_conf.h
 *  用来控制系统的一些变量，主要以 define 的形式存在
 */

// Warn: 特化机器人代码前，必须修改此处
#define ROBOT_BRIEF "DEV_ROBOT"

// 若启用，则可通过串口访问终端
#define USE_TERMINAL

#define TERMINAL_USER_NAME "user"
#define TERMINAL_PLATFORM_NAME "stm32"

// 【试验性功能】 若启用，则通过 Flash 检测代码类型是否一致 (判断 ROBOT_BRIEF 和 sys_type 是否相同，专治烧错代码)
#define USE_FLASH_CHECK

// 若启用，则从 E_UART_REFEREE 更新裁判系统相关信息，注意要在主函数中初始化串口
#define USE_REFEREE_SYSTEM

#define SYS_FLASH_KEY 998244353

// 底盘
// #define COMPILE_CHASSIS

// 云台
// #define COMPILE_GIMBAL

// 若不是同时编译云台和底盘，则启用双控制器选项
#if !(defined(COMPILE_CHASSIS) && defined(COMPILE_GIMBAL))
#define USE_DUAL_CONTROLLER
#endif

#endif //APP_CONF_H
