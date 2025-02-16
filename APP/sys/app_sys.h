//
// Created by fish on 2024/11/3.
//

#pragma once
#include "main.h"
#include "app_conf.h"

#include <string>

bool app_sys_ready();

struct app_sys_flash_t {
    int flag = 0;
    char brief[32] = "";
    int type = 0;
} __attribute__((__packed__));

struct app_sys_conf_t {
    char brief[32] = ROBOT_BRIEF;
    int type       = 0;
};

inline char app_sys_type_str[4][20] = {
    "NONE",
    "CHASSIS",
    "GIMBAL",
    "CHASSIS/GIMBAL"
};

const app_sys_conf_t *app_sys_conf();

#ifdef __cplusplus
extern "C" {
#endif

/*!
* 系统任务
*/
void app_sys_task();
__weak void app_chassis_task(void *args);
__weak void app_gimbal_task(void *args);
__weak void dev_dji_motor_task(void *args);
__weak void app_ins_task(void *args);

#ifdef __cplusplus
}
#endif