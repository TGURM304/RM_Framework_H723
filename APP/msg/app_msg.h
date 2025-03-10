//
// Created by fish on 2024/11/17.
//

#ifndef APP_MSG_H
#define APP_MSG_H

#include <cstdint>
#include <initializer_list>
#include <vector>
#include <algorithm>
#include "bsp_uart.h"


/*!
 * 通过 Vofa+ 的 Justfloat 协议发送调试数据（所有数据统一转换为 float）
 * @param e 串口枚举类
 * @param args 待发送的数据，可包含不同类型，但最终都会转换为 float
 * @note app_msg_vofa_send(E_UART_DEBUG, data1, data2, data3);
 */
template <typename... Args>
void app_msg_vofa_send(bsp_uart_e e, Args... args) {
    union {
        const uint8_t ch[4] = { 0x00, 0x00, 0x80, 0x7f };
        float f;
    } tail;
    std::vector<float> buffer { static_cast<float>(args)..., tail.f };
    bsp_uart_send(e, reinterpret_cast<uint8_t*>(buffer.data()), buffer.size() * sizeof(float));
}


#endif //APP_MSG_H