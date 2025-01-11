//
// Created by fish on 2024/11/16.
//

#ifndef APP_CONF_H
#define APP_CONF_H

/*
 *  app_conf.h
 *  用来控制系统的一些变量，主要以 define 的形式存在
 */

// 底盘
// #define COMPILE_CHASSIS

// 云台
#define COMPILE_GIMBAL

// 若不是同时编译云台和底盘，则启用双控制器选项
#if !(defined(COMPILE_CHASSIS) && defined(COMPILE_GIMBAL))
#define USE_DUAL_CONTROLLER
#endif

#endif //APP_CONF_H
