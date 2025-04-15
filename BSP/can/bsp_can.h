//
// Created by fish on 2024/11/15.
//

#ifndef BSP_CAN_H
#define BSP_CAN_H

#define BSP_CAN_FILTER_LIMIT 25
#define BSP_CAN_MSG_LIMIT 64

#include "stdint.h"
#include "fdcan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BSP_CAN_ENUM_SIZE E_CAN_END_DONT_REMOVE

typedef enum {
    E_CAN1,
    E_CAN2,
    E_CAN3,
    E_CAN_END_DONT_REMOVE
} bsp_can_e;

typedef struct {
    bsp_can_e port;
    FDCAN_RxHeaderTypeDef header;
    uint8_t data[BSP_CAN_MSG_LIMIT];
} bsp_can_msg_t;

void bsp_can_init(bsp_can_e e, FDCAN_HandleTypeDef *h);
uint8_t bsp_can_set_callback(bsp_can_e e, uint32_t id, void (*f) (bsp_can_msg_t *msg));
void bsp_can_send(bsp_can_e e, uint32_t id, uint8_t *s);
void bsp_can_fd_send(bsp_can_e e, uint32_t id, uint8_t *s, uint8_t l);

#ifdef __cplusplus
}
#endif

#endif //BSP_CAN_H
