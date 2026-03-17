//
// Created by fish on 2025/9/19.
//

#pragma once

#include "bsp_def.h"

#define BSP_UART_DEVICE_COUNT 6
#define BSP_UART_BUFFER_SIZE 512

typedef enum {
    E_UART_1,
    E_UART_2,
    E_UART_3,
    E_UART_5,
    E_UART_7,
    E_UART_10
} bsp_uart_e;

#ifdef __cplusplus
extern "C" {
#endif

    // 串口回调函数类型定义
    typedef void (*bsp_uart_callback_t) (bsp_uart_e device, const uint8_t *data, size_t len);
    // 串口阻塞发送
    void bsp_uart_send(bsp_uart_e device, const uint8_t *data, uint32_t len);
    // 串口异步发送
    void bsp_uart_send_async(bsp_uart_e device, const uint8_t *data, uint32_t len);
    // 串口阻塞 printf
    void bsp_uart_printf(bsp_uart_e device, const char *fmt, ...);
    // 串口异步 printf
    void bsp_uart_printf_async(bsp_uart_e device, const char *fmt, ...);
    // 设置串口回调并开启对应串口的空闲终端
    void bsp_uart_set_callback(bsp_uart_e device, bsp_uart_callback_t func);
    // 设置串口波特率
    void bsp_uart_set_baudrate(bsp_uart_e device, uint32_t baudrate);

#ifdef __cplusplus
}
#endif