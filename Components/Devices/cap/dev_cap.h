//
// Created by fish on 2025/2/13.
//

#pragma once

#include <cstdint>

namespace CAP {
    struct cap_raw_data_t {
        uint16_t cap_voltage;
        int16_t cap_current;
        uint8_t input_power;
        uint8_t limit_power;
        uint8_t cap_percent;
        bool cap_pid_running : 1;
        bool cap_under_voltage : 1;
        uint8_t cap_status_flag : 2;
        bool cap_power_overflow : 1;
    } __attribute__((__packed__));

    struct cap_data_t {
        cap_raw_data_t raw;
        float cap_voltage, cap_current;
        uint8_t input_power, limit_power, cap_percent;
        unsigned int last_online_time;
    };

    void init();
    void send(float limit);
    const cap_data_t *data();
}