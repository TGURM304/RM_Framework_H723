//
// Created by fish on 2024/9/6.
//

#ifndef BSP_TIME_H
#define BSP_TIME_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t bsp_time_get_ms();
uint64_t bsp_time_get_us();
// 使用 bsp_time_get_ms 检查时间戳是否超时
bool bsp_time_valid(unsigned int ts, unsigned int timeout);

#ifdef __cplusplus
}
#endif

#endif //BSP_TIME_H
