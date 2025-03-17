//
// Created by fish on 2024/10/30.
//

#include "bsp_rc.h"

#include "bsp_def.h"
#include "bsp_time.h"
#include "bsp_uart.h"

#include "usart.h"

#include "string.h"

#define RC_UART_PORT huart5

/*
 *  遥控器原始数据包，禁止从外部直接访问或修改。
 */
typedef struct {
    uint16_t ch0 : 11, ch1 : 11, ch2 : 11, ch3 : 11;
    uint8_t s1 : 2, s2 : 2;
    int16_t mouse_x : 16, mouse_y : 16, mouse_z : 16;
    uint8_t mouse_l : 8, mouse_r : 8;
    uint16_t keyboard : 16, reserved : 16;
} __attribute__((packed)) bsp_rc_raw_t;

static bsp_rc_raw_t raw;
bsp_rc_data_t data;

void rc_uart_callback(bsp_uart_e e, uint8_t *s, uint16_t l) {
    BSP_ASSERT(e == E_UART_RC);
    BSP_ASSERT(sizeof(bsp_rc_raw_t) * 8 == 128 + 16);
    memcpy(&raw, s, sizeof raw);
    data.rc_l[0] = (int16_t) ((int16_t) raw.ch2 - 1024), data.rc_l[1] = (int16_t) ((int16_t) raw.ch3 - 1024);
    data.rc_r[0] = (int16_t) ((int16_t) raw.ch0 - 1024), data.rc_r[1] = (int16_t) ((int16_t) raw.ch1 - 1024);
    data.s_l = (int8_t) (raw.s2 == 3 ? 0 : raw.s2 == 2 ? -1 : 1);
    data.s_r = (int8_t) (raw.s1 == 3 ? 0 : raw.s1 == 2 ? -1 : 1);
    data.mouse_x = raw.mouse_x, data.mouse_y = raw.mouse_y, data.mouse_z = raw.mouse_z;
    data.mouse_l = raw.mouse_l, data.mouse_r = raw.mouse_r;
    data.keyboard.raw = raw.keyboard, data.reserved = (int16_t) (1024 - raw.reserved);
    data.timestamp = bsp_time_get_ms();
}

const bsp_rc_data_t *bsp_rc_data() {
    return &data;
}

void bsp_rc_init() {
    bsp_uart_init(E_UART_RC, &RC_UART_PORT);
    bsp_uart_set_callback(E_UART_RC, rc_uart_callback);
}