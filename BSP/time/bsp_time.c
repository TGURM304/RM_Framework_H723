//
// Created by fish on 2024/9/6.
//

#include "bsp_time.h"

/**
 * @brief  获取系统运行的毫秒数
 * @return 当前系统 Tick (ms)
 * @note   基于 FreeRTOS 的 xTaskGetTickCount()
 */
uint32_t bsp_time_get_ms() {
    return xTaskGetTickCount();
}

/**
 * @brief  获取系统运行的微秒数
 * @return 当前系统时间 (us)
 * @note   通过 Tick + SysTick 寄存器计算，精度高于毫秒计数
 */
uint64_t bsp_time_get_us() {
    uint32_t ms_old = xTaskGetTickCount();
    uint32_t tick_val_old = SysTick->VAL;
    uint32_t ms_now = xTaskGetTickCount();
    uint32_t tick_val_now = SysTick->VAL;
    return ms_old == ms_now ?
        ms_now * 1000 + 1000 - tick_val_old * 1000 / (SysTick->LOAD + 1):
        ms_now * 1000 + 1000 - tick_val_now * 1000 / (SysTick->LOAD + 1);
}

/**
 * @brief  检查某个时间戳是否在有效超时范围内
 * @param  ts       起始时间戳 (ms)
 * @param  timeout  超时时间 (ms)
 * @return true     未超时
 * @return false    已超时
 */
bool bsp_time_valid(unsigned int ts, unsigned int timeout) {
    return bsp_time_get_ms() - ts <= timeout;
}
