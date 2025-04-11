//
// Created by fish on 2024/9/2.
//

#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "stdint.h"
#include "bsp_uart.h"
#include "cmsis_os2.h"
#include "bsp_led.h"
#include "bsp_sys.h"
#include "bsp_buzzer.h"

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((unused)) static void bsp_assert_err(const char *file, uint32_t line) {
    UNUSED(file);
    UNUSED(line);
    // 开启 rtos 调度锁，强行停止其他任务，便于调试。
    vTaskSuspendAll();
    bsp_led_set(255, 0, 0);
    bsp_buzzer_quiet();
    while(1)
        __NOP();
}

#define BSP_ASSERT(arg)                                                                                                \
    do {                                                                                                               \
        if(!(arg)) bsp_assert_err(__FILE__, __LINE__);                                                                 \
    } while(0)

typedef enum {
    BSP_OK  = 0,
    BSP_ERR = 1,
} bsp_status_t;

#ifdef __cplusplus
}
#endif
