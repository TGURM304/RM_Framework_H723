//
// Created by fish on 2025/9/24.
//

#pragma once

#include "bsp_def.h"

#define BSP_CAN_DEVICE_COUNT 3
#define BSP_CAN_FILTER_LIMIT_STD 24
#define BSP_CAN_BUFFER_SIZE 64

typedef enum {
    E_CAN_1,
    E_CAN_2,
    E_CAN_3
} bsp_can_e;

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*bsp_can_callback_t)(bsp_can_e device, uint32_t id, const uint8_t *data, size_t len);
void bsp_can_init(bsp_can_e device);
void bsp_can_set_callback(bsp_can_e device, uint32_t id, bsp_can_callback_t func);
void bsp_can_send(bsp_can_e device, uint32_t id, const uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif
