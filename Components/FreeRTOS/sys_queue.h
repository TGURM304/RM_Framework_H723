//
// Created by fish on 2025/1/25.
//

#pragma once
#include "bsp_sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

namespace OS {
    template <typename T>
    class Queue {
    public:
        explicit Queue(uint16_t length) : queue_(xQueueCreate(length, sizeof(T))) {}

        bool send(const T& data) {
            if(bsp_sys_in_isr()) {
                BaseType_t xHigherPriorityTaskWoken;
                auto re = xQueueSendFromISR(queue_, &data, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                return re == pdTRUE;
            } else {
                return xQueueSend(queue_, &data, 0) == pdTRUE;
            }
        }

        bool receive(T& data) {
            if(bsp_sys_in_isr()) {
                BaseType_t xHigherPriorityTaskWoken;
                auto re = xQueueReceiveFromISR(queue_, &data, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                return re == pdTRUE;
            } else {
                return xQueueReceive(queue_, &data, 0) == pdTRUE;
            }
        }

        bool overwrite(T &data) {
            if(bsp_sys_in_isr()) {
                BaseType_t xHigherPriorityTaskWoken;
                auto re = xQueueOverwriteFromISR(queue_, &data, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                return re == pdTRUE;
            } else {
                return xQueueOverwrite(queue_, &data) == pdTRUE;
            }
        }

        bool reset() { return xQueueReset(queue_) == pdTRUE; }

        uint32_t size() {
            return bsp_sys_in_isr() ?
                uxQueueMessagesWaitingFromISR(queue_) : uxQueueMessagesWaiting(queue_);
        }
    private:
        QueueHandle_t queue_ = nullptr;
    };
}
