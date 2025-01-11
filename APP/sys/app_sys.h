//
// Created by fish on 2024/11/3.
//

#pragma once
#include "main.h"

bool app_sys_ready();

#ifdef __cplusplus
extern "C" {
#endif

void app_sys_init();
void app_sys_task();
__weak void app_chassis_task(void *argument);
__weak void app_gimbal_task(void *argument);
__weak void dev_dji_motor_task(void *argument);
__weak void app_ins_task(void *argument);

#ifdef __cplusplus
}
#endif