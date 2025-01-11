//
// Created by fish on 2024/11/16.
//

#pragma once

#include "app_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

void app_gimbal_init();
void app_gimbal_task(void *args);

#ifdef __cplusplus
}
#endif