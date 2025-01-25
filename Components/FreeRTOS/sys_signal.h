//
// Created by fish on 2025/1/25.
//

#pragma once

#include <sys_task.h>

#include "bsp_def.h"

namespace OS {
    class Signal {
    public:
        static bool action(Task& task, int sig) {
            BSP_ASSERT(0 <= sig and sig < 32);
            if(bsp_sys_in_isr()) {
                BaseType_t xHigherPriorityTaskWoken;
                auto re = xTaskNotifyFromISR(task.handle_, 1 << sig, eSetBits, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                return re == pdTRUE;
            } else {
                return xTaskNotify(task.handle_, 1 << sig, eSetBits) == pdTRUE;
            }
        }

        static bool wait(int sig, uint32_t timeout = UINT32_MAX) {
            BSP_ASSERT(0 <= sig and sig < 32);
            uint32_t value = 0;
            auto task = xTaskGetCurrentTaskHandle();
            xTaskNotifyAndQuery(task, 0, eNoAction, &value);

            if(value >> sig & 1) {
                value &= ~(1 << sig);
                xTaskNotify(task, value, eSetValueWithOverwrite);
                return true;
            }

            if(timeout == 0) return false;

            uint32_t current_time = xTaskGetTickCount();
            while(xTaskNotifyWait(0, 1 << sig, &value, timeout) == pdTRUE) {
                if(value >> sig & 1) {
                    return true;
                }

                uint32_t now = xTaskGetTickCount();

                if (now - current_time >= timeout) {
                    return false;
                }

                timeout -= now - current_time;
            }

            return false;
        }
    };
}
