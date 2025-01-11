//
// Created by fish on 2024/9/6.
//

#include "bsp_time.h"

uint32_t bsp_time_get_ms() {
    return xTaskGetTickCount();
}

uint64_t bsp_time_get_us() {
    uint32_t ms_old = xTaskGetTickCount();
    uint32_t tick_val_old = SysTick->VAL;
    uint32_t ms_now = xTaskGetTickCount();
    uint32_t tick_val_now = SysTick->VAL;
    return ms_old == ms_now ?
        ms_now * 1000 + 1000 - tick_val_old * 1000 / (SysTick->LOAD + 1):
        ms_now * 1000 + 1000 - tick_val_now * 1000 / (SysTick->LOAD + 1);
}

uint64_t bsp_time_get() __attribute__((alias("bsp_time_get_us")));
