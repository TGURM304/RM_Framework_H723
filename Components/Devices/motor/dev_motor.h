//
// Created by fish on 2024/11/15.
//

#pragma once

typedef struct {
    float speed, angle, current, temperature;
    unsigned int last_online_time;
} MotorStatus;

namespace Motor {
    class Base {
    public:
        virtual ~Base() = default;
        virtual void init() = 0;
        virtual void enable() = 0;
        virtual void disable() = 0;
        virtual void update(float output) = 0;
        MotorStatus status;
    protected:
        bool enabled_ = false;
    };
}