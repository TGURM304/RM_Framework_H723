//
// Created by fish on 2025/2/13.
//

#include "dev_cap.h"

#include <cstring>

#include "bsp_can.h"
#include "bsp_time.h"

using namespace CAP;

static cap_data_t data_;

const cap_data_t* CAP::data() {
    return &data_;
}

#define reverse(x) { x = (x & 0xff) << 8 | x >> 8; }

void recv(bsp_can_e device, uint32_t id, const uint8_t *data, size_t len) {
    memcpy(&data_.raw, data, sizeof(cap_raw_data_t));
    reverse(data_.raw.cap_current);
    reverse(data_.raw.cap_voltage);
    // data_.cap_voltage = static_cast <float> (data_.raw.cap_voltage) / 1e3f;
    // data_.cap_current = static_cast <float> (data_.raw.cap_current) / 1e3f;
    data_.cap_voltage = static_cast<float>(data[0] << 8 | data[1]) /1000.0f;
    data_.cap_current = static_cast<float>(static_cast<int16_t>(data[2] << 8 | data[3])) /1000.0f;
    data_.input_power = data_.raw.input_power;
    data_.limit_power = data_.raw.limit_power;
    data_.cap_percent = data_.raw.cap_percent;
    data_.last_online_time = bsp_time_get_ms();
}

void CAP::send(float limit) {
    int x = static_cast <int> (limit), y = static_cast <int> (limit * 100) % 100;
    // 30 <= x <= 250, 0 <= y <= 99
    uint8_t tx[8] = { 0 };
    tx[0] = x, tx[1] = y;
    bsp_can_send(E_CAN_1, 0x0ff, tx, 8);
}

void CAP::init() {
    bsp_can_set_callback(E_CAN_1, 0x0ff, recv);
}