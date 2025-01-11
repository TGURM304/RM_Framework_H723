//
// Created by fish on 2024/11/17.
//

#include "app_msg.h"

#include "app_ins.h"
#include "bsp_can.h"
#include "bsp_led.h"

#include "app_conf.h"

#include "sys_task.h"

#include <cstring>
#include <initializer_list>

#include "bsp_def.h"

/*
 *  app_msg
 *  实现对部分通信协议的封装。
 */

/*
 *  Vofa+ Justfloat
 */

float ch[APP_MSG_VOFA_CHANNEL_LIMIT];
uint8_t vofa_tail[4] = {0x00, 0x00, 0x80, 0x7f};

void app_msg_vofa_send(bsp_uart_e e, std::initializer_list <double> f) {
    BSP_ASSERT(0 < f.size() and f.size() <= APP_MSG_VOFA_CHANNEL_LIMIT);
    uint8_t p = 0;
    for(auto &i : f) {
        ch[p ++] = static_cast <float> (i);
    }
    bsp_uart_send(e, reinterpret_cast <uint8_t *> (&ch), f.size() * sizeof(float));
    bsp_uart_send(e, vofa_tail, sizeof vofa_tail);
}