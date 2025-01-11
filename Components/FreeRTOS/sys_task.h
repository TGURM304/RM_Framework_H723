//
// Created by fish on 2024/11/3.
//

#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "sys_ext.h"

namespace OS {
    class Task {
    public:
        typedef enum { IDLE, LOW, MEDIUM, HIGH, REALTIME } Priority;
        TaskHandle_t handle_ = nullptr;

        Task() = default;
        explicit Task(TaskHandle_t h) : handle_(h) {};

        template <typename Fn, typename Arg>
        Task(Fn fn, Arg arg, const char *name, uint32_t stack_depth, Priority priority) {
            Create(fn, arg, name, stack_depth, priority);
        }

        template <typename Fn, typename Arg>
        void Create(Fn fn, Arg arg, const char *name, uint32_t stack_depth, Priority priority) {
            (void) static_cast<void (*)(Arg)> (fn);
            auto *type = static_cast <TypeErasure <void, Arg> *> (
                pvPortMalloc(sizeof (TypeErasure <void, Arg>))
            );
            *type = TypeErasure <void, Arg> (fn, arg);
            xTaskCreate(type->Port, name, stack_depth, type, priority, &(this->handle_));
        }

        static void Sleep(uint32_t ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }
        static void SleepMilliseconds(uint32_t ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }
        static void SleepSeconds(uint32_t s) { vTaskDelay(pdMS_TO_TICKS(s * 1000)); }
        static void SleepMinutes(uint32_t m) { SleepSeconds(m * 60); }
        static void SleepHours(uint32_t h) { while(h--) SleepMinutes(60); }
        static void SleepDays(uint32_t d) { while(d--) SleepHours(24); }
        static void SleepUntil(uint32_t ms, uint32_t &last_weak_up_time) { vTaskDelayUntil(&last_weak_up_time, ms); }

        static Task Current() { return Task(xTaskGetCurrentTaskHandle()); }
        void Delete() const { vTaskDelete(this->handle_); }
        static void Yield() { taskYIELD(); }
    };
}
