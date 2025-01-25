//
// Created by fish on 2025/1/25.
//

#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

namespace OS {
    class Mutex {
    public:
        Mutex() : mutex_(xSemaphoreCreateMutex()) {}
        ~Mutex() { vSemaphoreDelete(mutex_); }
        void unlock() { xSemaphoreGive(mutex_); }
        bool lock() { return xSemaphoreTake(mutex_, UINT32_MAX); }
    private:
        SemaphoreHandle_t mutex_;
    };
}
