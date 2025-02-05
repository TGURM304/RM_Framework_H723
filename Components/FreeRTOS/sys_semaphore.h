//
// Created by fish on 2025/1/25.
//

#pragma once
#include <cstdint>

#include "bsp_sys.h"
#include "FreeRTOS.h"
#include "semphr.h"

namespace OS {
    class Semaphore {
    public:
        explicit Semaphore(uint32_t init_count) : handle_(xSemaphoreCreateCounting(UINT32_MAX, init_count)) {}
        ~Semaphore() { vSemaphoreDelete(handle_); }

        void post() {
            if(bsp_sys_in_isr()) {
                BaseType_t xHigherPriorityTaskWoken;
                xSemaphoreGiveFromISR(handle_, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            } else {
                xSemaphoreGive(handle_);
            }
        }

        bool wait(uint32_t timeout = UINT32_MAX) {
            if(bsp_sys_in_isr()) {
                BaseType_t xHigherPriorityTaskWoken;
                auto re = xSemaphoreTakeFromISR(handle_, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                return re == pdTRUE;
            } else {
                return xSemaphoreTake(handle_, timeout) == pdTRUE;
            }
        }

        uint32_t value() { return uxSemaphoreGetCount(handle_); }
    private:
        SemaphoreHandle_t handle_;
    };
}
