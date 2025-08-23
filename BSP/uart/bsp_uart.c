//
// Created by fish on 2024/9/2.
//

#include "bsp_uart.h"
#include "bsp_def.h"
#include "stdio.h"
#include "string.h"
#include "usbd_cdc_if.h"

static uint8_t uart_tx_buf[UART_BUFFER_SIZE];
static uint8_t uart_rx_buf[UART_ENUM_SIZE][UART_BUFFER_SIZE];
static UART_HandleTypeDef *handle[UART_ENUM_SIZE] = { NULL };
static void (*callback[UART_ENUM_SIZE])(bsp_uart_e e, uint8_t *s, uint16_t l);

/**
 * @brief  初始化 UART 接口
 * @param  e  UART 端口（枚举值）
 * @param  h  HAL UART 句柄
 * @note   CDC (USB) 不使用 HAL UART 句柄
 */
void bsp_uart_init(bsp_uart_e e, UART_HandleTypeDef *h) {
    BSP_ASSERT(handle[e] == NULL);
    if(e != E_UART_CDC) handle[e] = h;
}
/**
 * @brief  设置 UART 接收回调函数
 * @param  e  UART 端口（枚举值）
 * @param  f  接收数据回调函数，参数为 (端口, 数据指针, 长度)
 * @note   非 CDC 使用 DMA+空闲中断接收数据
 */
void bsp_uart_set_callback(bsp_uart_e e, void (*f)(bsp_uart_e e, uint8_t *s, uint16_t l)) {
    BSP_ASSERT(callback[e] == NULL);
    BSP_ASSERT(e == E_UART_CDC || handle[e]);
    callback[e] = f;
    if(e != E_UART_CDC) {
        HAL_UARTEx_ReceiveToIdle_DMA(handle[e], uart_rx_buf[e], UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(handle[e]->hdmarx, DMA_IT_HT);
    }
}
/**
 * @brief  非阻塞发送 UART 数据
 * @param  e  UART 端口（枚举值）
 * @param  s  数据缓冲区
 * @param  l  数据长度
 * @note   USB CDC 和普通 UART 都支持
 */
void bsp_uart_send(bsp_uart_e e, uint8_t *s, uint16_t l) {
    BSP_ASSERT(e == E_UART_CDC || handle[e]);
    if(e == E_UART_CDC) {
        // USB CDC
        CDC_Transmit_HS(s, l);
    } else {
        // UART
        HAL_UART_Transmit_DMA(handle[e], s, l);
    }
}
/**
 * @brief  阻塞发送 UART 数据
 * @param  e  UART 端口（枚举值）
 * @param  s  数据缓冲区
 * @param  l  数据长度
 * @note   USB CDC 和普通 UART 都支持
 */
void bsp_uart_send_block(bsp_uart_e e, uint8_t *s, uint16_t l) {
    BSP_ASSERT(e == E_UART_CDC || handle[e]);
    if(e == E_UART_CDC) {
        // USB CDC
        CDC_Transmit_HS(s, l);
    } else {
        // UART
        HAL_UART_Transmit(handle[e], s, l, HAL_MAX_DELAY);
    }
}
/**
 * @brief  格式化输出到 UART (非阻塞)
 * @param  e    UART 端口（枚举值）
 * @param  fmt  格式化字符串
 * @note   内部使用 vsnprintf 格式化，再调用 bsp_uart_send
 */
void bsp_uart_printf(bsp_uart_e e, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    uint16_t len = vsnprintf(uart_tx_buf, UART_BUFFER_SIZE, fmt, ap);
    va_end(ap);
    bsp_uart_send(e, uart_tx_buf, len);
}
/**
 * @brief  格式化输出到 UART (阻塞)
 * @param  e    UART 端口（枚举值）
 * @param  fmt  格式化字符串
 * @note   内部使用 vsnprintf 格式化，再调用 bsp_uart_send_block
 */
void bsp_uart_printf_block(bsp_uart_e e, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    uint16_t len = vsnprintf(uart_tx_buf, UART_BUFFER_SIZE, fmt, ap);
    va_end(ap);
    bsp_uart_send_block(e, uart_tx_buf, len);
}
/**
 * @brief  UART 接收完成回调 (HAL 内部自动调用)
 * @note   自动调用注册的回调函数处理接收到的数据，并清空缓冲
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *h, uint16_t l) {
    for(int i = 0; i < UART_ENUM_SIZE; i++) {
        if(h == handle[i]) {
            if(callback[i] != NULL) callback[i](i, uart_rx_buf[i], l);
            memset(uart_rx_buf[i], 0, sizeof(uint8_t) * l);
            HAL_UARTEx_ReceiveToIdle_DMA(h, uart_rx_buf[i], UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(h->hdmarx, DMA_IT_HT);
        }
    }
}
/**
 * @brief  UART 错误回调 (HAL 内部调用)
 * @param  h  HAL UART 句柄
 * @note   出错时重新启动 DMA 接收，防止接收中断丢失
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *h) {
    for(int i = 0; i < UART_ENUM_SIZE; i++) {
        if(handle[i] == NULL) continue;
        if(h == handle[i]) {
            HAL_UARTEx_ReceiveToIdle_DMA(h, uart_rx_buf[i], UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(h->hdmarx, DMA_IT_HT);
            break;
        }
    }
}
/**
 * @brief  USB CDC 接收回调函数
 * @param  s  接收到的数据
 * @param  l  数据长度
 * @note   内部调用注册的 CDC 回调函数
 */
void usb_cdc_callback(uint8_t *s, uint16_t l) {
    if(callback[E_UART_CDC] != NULL) callback[E_UART_CDC](E_UART_CDC, s, l);
}