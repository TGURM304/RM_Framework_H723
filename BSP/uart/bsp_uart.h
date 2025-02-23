//
// Created by fish on 2024/9/2.
//

#ifndef BSP_UART_H
#define BSP_UART_H

#include "usart.h"
#include "stdarg.h"

#define UART_BUFFER_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif

// 注：USB CDC 暂时无法连续发送，每次发送之间需要有 1ms 间隔，且无需在这里 init
typedef enum {
    E_UART_DEBUG,
    E_UART_RC,
    E_UART_VISION,
    E_UART_CDC,   // USB CDC
    E_UART_485,
    E_UART_END
} bsp_uart_e;

#define UART_ENUM_SIZE (E_UART_END + 1)

void bsp_uart_init(bsp_uart_e e, UART_HandleTypeDef *h);
void bsp_uart_send(bsp_uart_e e, uint8_t *s, uint16_t l);
void bsp_uart_printf(bsp_uart_e e, const char *fmt, ...);
void bsp_uart_set_callback(bsp_uart_e e, void (*f)(bsp_uart_e e, uint8_t *s, uint16_t l));
void usb_cdc_callback(uint8_t *s, uint16_t l);

#ifdef __cplusplus
}
#endif

#endif //BSP_UART_H
