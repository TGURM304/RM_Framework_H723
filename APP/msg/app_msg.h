//
// Created by fish on 2024/11/17.
//

#ifndef APP_MSG_H
#define APP_MSG_H

#include "alg_crc.h"
#include "bsp_can.h"
#include "bsp_time.h"


#include <cstdint>
#include <vector>
#include "bsp_uart.h"

#include <algorithm>
#include <array>
#include <functional>

// 根据总线负载选择
#define MSG_CAN_LIMIT_PER_MILLISECOND 8

/*!
 * 通过 Vofa+ 的 Justfloat 协议发送调试数据（所有数据统一转换为 float）
 * @param e 串口枚举类
 * @param args 待发送的数据，可包含不同类型，但最终都会转换为 float
 * @note app_msg_vofa_send(E_UART_DEBUG, data1, data2, data3);
 */
template <typename... Args> void app_msg_vofa_send(bsp_uart_e e, Args... args) {
    union {
        const uint8_t ch[4] = { 0x00, 0x00, 0x80, 0x7f };
        float f;
    } tail;
    std::vector<float> buffer{ static_cast<float>(args)..., tail.f };
    bsp_uart_send(e, reinterpret_cast<uint8_t *>(buffer.data()), buffer.size() * sizeof(float));
}

struct app_msg_can_t {
    bsp_can_e port;
    uint32_t id;
    std::array <uint8_t, 8> data;
};

void app_msg_can_set_callback(bsp_can_e e, uint32_t id, std::function<void(uint8_t*, uint16_t)> callback);

// 原始函数，无 CRC 封包
void app_msg_can_send(bsp_can_e e, uint32_t id, uint8_t *s);

template <typename T>
void app_msg_can_send(bsp_can_e e, uint32_t id, T &data) {
    size_t k = std::ceil((sizeof(T) + 4) / 8.0) * 8;
    std::vector <uint8_t> pkg(k);
    pkg[0] = 0xa5, pkg[1] = 0x5a, pkg[2] = sizeof(data);
    std::copy_n(reinterpret_cast<uint8_t *>(&data), sizeof(T), pkg.begin() + 3);
    pkg[sizeof(T) + 3] = CRC8::calc(pkg.data(), sizeof(T) + 3);
    for(size_t i = 0; i < pkg.size(); i += 8) {
        app_msg_can_send(e, id, pkg.data() + i);
    }
}

template <typename T>
class app_msg_can_receiver {
public:
    app_msg_can_receiver(bsp_can_e e, uint32_t id) : port(e), id(id) {}
    void init() {
        auto recv = [this](uint8_t *s, uint16_t l) {
            if(l != sizeof(T)) return;
            timestamp = bsp_time_get_ms();
            std::copy_n(s, sizeof(T), reinterpret_cast<uint8_t *>(&data));
        };
        app_msg_can_set_callback(port, id, recv);
    }
    unsigned int timestamp = 0;
    const T* operator () () { return &data; }
    [[nodiscard]] bool valid() const { return bsp_time_get_ms() - timestamp < 100; }
private:
    T data;
    bsp_can_e port;
    uint32_t id;
};

#endif //APP_MSG_H