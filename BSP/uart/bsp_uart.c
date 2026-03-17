//
// Created by fish on 2025/9/19.
//

#include "bsp_uart.h"
#include "bsp_ds.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "usart.h"

static UART_HandleTypeDef *handle[BSP_UART_DEVICE_COUNT] = {
    [E_UART_1] = &huart1,
    [E_UART_2] = &huart2,
    [E_UART_3] = &huart3,
    [E_UART_5] = &huart5,
    [E_UART_7] = &huart7,
    [E_UART_10] = &huart10
};

// Ring Queue for async transmission
static ds_rq_t rq[BSP_UART_DEVICE_COUNT];
static volatile uint8_t busy[BSP_UART_DEVICE_COUNT];
static uint8_t rq_buffer[BSP_UART_DEVICE_COUNT][BSP_UART_BUFFER_SIZE];

static bsp_uart_callback_t callback[BSP_UART_DEVICE_COUNT];
static uint8_t tx_buffer[BSP_UART_DEVICE_COUNT][BSP_UART_BUFFER_SIZE];
static uint8_t rx_buffer[BSP_UART_DEVICE_COUNT][BSP_UART_BUFFER_SIZE];

void bsp_uart_send(bsp_uart_e device, const uint8_t *data, const uint32_t len) {
    BSP_ASSERT(0 <= device && device < BSP_UART_DEVICE_COUNT && data != NULL && len > 0);
    HAL_UART_Transmit(handle[device], data, len, HAL_MAX_DELAY);
}

void bsp_uart_send_async(bsp_uart_e device, const uint8_t *data, const uint32_t len) {
    BSP_ASSERT(0 <= device && device < BSP_UART_DEVICE_COUNT && data != NULL && len > 0 && len <= BSP_UART_BUFFER_SIZE);

    uint8_t send_now = 0;
    unsigned int _state = bsp_sys_enter_critical();

    if (rq[device].buf != rq_buffer[device])
        ds_rq_init(&rq[device], rq_buffer[device], BSP_UART_BUFFER_SIZE);

    if (!busy[device]) {
        busy[device] = 2;                                               // 这里 busy 状态为 2 表示等待发送, 外设并未实际开始发送
        send_now = 1;
    } else {
        if (ds_rq_avail(&rq[device]) >= sizeof(len) + len) {
            ds_rq_push(&rq[device], (const uint8_t *) &len, sizeof(len));
            ds_rq_push(&rq[device], data, len);
        } else {
            ;                                                           // drop
        }
    }

    bsp_sys_exit_critical(_state);

    if (send_now) {
        if (data != tx_buffer[device]) {
            memcpy(tx_buffer[device], data, len);
            data = tx_buffer[device];
        }
        HAL_StatusTypeDef st;
        if (handle[device]->hdmatx) {
            st = HAL_UART_Transmit_DMA(handle[device], data, len);
        } else {
            st = HAL_UART_Transmit_IT(handle[device], data, len);
        }
        _state = bsp_sys_enter_critical();
        busy[device] = (st == HAL_OK);
        bsp_sys_exit_critical(_state);
    }
}

void bsp_uart_printf(bsp_uart_e device, const char *fmt, ...) {
    BSP_ASSERT(0 <= device && device < BSP_UART_DEVICE_COUNT);
    va_list ap;
    va_start(ap, fmt);
    uint8_t buf[BSP_UART_BUFFER_SIZE];
    const int len = vsnprintf((char *) buf, BSP_UART_BUFFER_SIZE, fmt, ap);
    va_end(ap);
    BSP_ASSERT(0 < len && len <= BSP_UART_BUFFER_SIZE);
    bsp_uart_send(device, buf, len);
}

void bsp_uart_printf_async(bsp_uart_e device, const char *fmt, ...) {
    BSP_ASSERT(0 <= device && device < BSP_UART_DEVICE_COUNT);
    va_list ap;
    va_start(ap, fmt);
    uint8_t buf[BSP_UART_BUFFER_SIZE];
    const int len = vsnprintf((char *) buf, BSP_UART_BUFFER_SIZE, fmt, ap);
    va_end(ap);
    BSP_ASSERT(0 < len && len <= BSP_UART_BUFFER_SIZE);
    bsp_uart_send_async(device, buf, len);
}

void bsp_uart_set_callback(bsp_uart_e device, bsp_uart_callback_t func) {
    BSP_ASSERT(0 <= device && device < BSP_UART_DEVICE_COUNT && callback[device] == NULL && func != NULL);
    BSP_ASSERT(handle[device]->hdmarx != NULL);     // 确保外设开启了 RX DMA

    callback[device] = func;

    HAL_UARTEx_ReceiveToIdle_DMA(handle[device], rx_buffer[device], BSP_UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(handle[device]->hdmarx, DMA_IT_HT);
}

// 不要在 set_callback 后执行 set_baudrate, 否则可能会破坏空闲中断状态
void bsp_uart_set_baudrate(bsp_uart_e device, uint32_t baudrate) {
    BSP_ASSERT(0 <= device && device < BSP_UART_DEVICE_COUNT && baudrate > 0 && callback[device] == NULL);

    if (!bsp_sys_in_isr()) {
        HAL_UART_StateTypeDef state = HAL_UART_GetState(handle[device]);
        while (state == HAL_UART_STATE_BUSY_TX || state == HAL_UART_STATE_BUSY_RX || state == HAL_UART_STATE_BUSY_TX_RX) {
            state = HAL_UART_GetState(handle[device]);
            osDelay(1);
        }
    }

    HAL_UART_DeInit(handle[device]);
    handle[device]->Init.BaudRate = baudrate;
    HAL_UART_Init(handle[device]);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *h, uint16_t len) {
    for (int i = 0; i < BSP_UART_DEVICE_COUNT; i++) {
        if (handle[i] == h) {
            if (callback[i] != NULL) {
                callback[i](i, rx_buffer[i], len);
                memset(rx_buffer[i], 0, sizeof(uint8_t) * len);
                HAL_UARTEx_ReceiveToIdle_DMA(h, rx_buffer[i], BSP_UART_BUFFER_SIZE);
                __HAL_DMA_DISABLE_IT(h->hdmarx, DMA_IT_HT);
            }
            break;
        }
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *h) {
    for (int i = 0; i < BSP_UART_DEVICE_COUNT; i++) {
        if (handle[i] == h && busy[i]) {
            uint32_t len = 0;
            uint8_t send_nxt = 0;
            unsigned long _state = bsp_sys_enter_critical();
            if (ds_rq_size(&rq[i]) == 0) {
                busy[i] = 0;
            } else {
                ds_rq_pop(&rq[i], (uint8_t *) &len, sizeof(len));
                ds_rq_pop(&rq[i], tx_buffer[i], len);
                if (len == 0 || len > BSP_UART_BUFFER_SIZE) {
                    busy[i] = 0;
                    ds_rq_init(&rq[i], rq_buffer[i], BSP_UART_BUFFER_SIZE);
                } else {
                    send_nxt = 1;
                }
            }
            bsp_sys_exit_critical(_state);
            if (send_nxt) {
                HAL_StatusTypeDef st;
                if (handle[i]->hdmatx) {
                    st = HAL_UART_Transmit_DMA(handle[i], tx_buffer[i], len);
                } else {
                    st = HAL_UART_Transmit_IT(handle[i], tx_buffer[i], len);
                }
                if (st != HAL_OK) {
                    _state = bsp_sys_enter_critical();
                    busy[i] = 0;
                    bsp_sys_exit_critical(_state);
                }
            }
            break;
        }
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *h) {
    for (int i = 0; i < BSP_UART_DEVICE_COUNT; i++) {
        if (handle[i] == h) {
            unsigned int _state = bsp_sys_enter_critical();
            ds_rq_init(&rq[i], rq_buffer[i], BSP_UART_BUFFER_SIZE);
            busy[i] = 0;
            bsp_sys_exit_critical(_state);
            HAL_UARTEx_ReceiveToIdle_DMA(h, rx_buffer[i], BSP_UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(h->hdmarx, DMA_IT_HT);
            break;
        }
    }
}