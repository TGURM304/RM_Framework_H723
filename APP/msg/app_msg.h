//
// Created by fish on 2024/11/17.
//

#ifndef APP_MSG_H
#define APP_MSG_H

#include "bsp_uart.h"
#include <cstdint>
#include <initializer_list>

#define APP_MSG_VOFA_CHANNEL_LIMIT 15

/*!
 * 通过串口以 vofa+ justfloat 协议发送调试数据
 * @param e 串口设备枚举类
 * @param f 浮点数据列表
 */
void app_msg_vofa_send(bsp_uart_e e, std::initializer_list <double> f);

#endif //APP_MSG_H
