//
// Created by fish on 2024/9/6.
//

#pragma once

#include "bsp_def.h"
#include "main.h"

__attribute__((unused)) static void bsp_sys_reset() {
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

__attribute__((unused)) static void bsp_sys_shutdown() {
    HAL_PWR_EnterSTANDBYMode();
}

__attribute__((unused)) static void bsp_sys_sleep() {
    HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

__attribute__((unused)) static void bsp_sys_stop() {
    HAL_PWR_EnterSTANDBYMode();
}

__attribute__((unused)) static uint8_t bsp_sys_in_isr() {
    uint32_t result;
    __asm__ volatile("MRS %0, ipsr" : "=r"(result));
    return (result);
}