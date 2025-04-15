//
// Created by fish on 2024/11/15.
//

#include "bsp_can.h"

#include "bsp_def.h"

static uint8_t tot;
static uint8_t cnt[E_CAN_END_DONT_REMOVE];

static FDCAN_HandleTypeDef *handle[E_CAN_END_DONT_REMOVE];

static uint32_t rx_id[E_CAN_END_DONT_REMOVE][BSP_CAN_FILTER_LIMIT];
static void (*callback[E_CAN_END_DONT_REMOVE][BSP_CAN_FILTER_LIMIT]) (bsp_can_msg_t *msg);

void bsp_can_init(bsp_can_e e, FDCAN_HandleTypeDef *h) {
    handle[e] = h;
    HAL_FDCAN_ActivateNotification(h, FDCAN_IT_TX_FIFO_EMPTY, 0);
    HAL_FDCAN_ActivateNotification(h, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(h, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
    HAL_FDCAN_ConfigGlobalFilter(h, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_REJECT_REMOTE);
    // 一定要在配置完后 Start，否则若总线上有 CAN 包，设备会 BUSY。
    HAL_FDCAN_Start(h);
}

uint8_t bsp_can_set_callback(bsp_can_e e, uint32_t id, void (*f) (bsp_can_msg_t *msg)) {
    BSP_ASSERT(tot < BSP_CAN_FILTER_LIMIT && f != NULL);
    rx_id[e][cnt[e]] = id;
    callback[e][cnt[e]] = f;

    FDCAN_FilterTypeDef filter = {
        .IdType = id > 0x7ff ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID,
        .FilterIndex = tot,
        .FilterType = FDCAN_FILTER_DUAL,
        .FilterID1 = id,
        .FilterID2 = id,
        .FilterConfig = (id & 1) ? FDCAN_FILTER_TO_RXFIFO0 : FDCAN_FILTER_TO_RXFIFO1,
    };

    BSP_ASSERT(HAL_FDCAN_ConfigFilter(handle[e], &filter) == HAL_OK);
    return tot ++, cnt[e] ++;
}

void bsp_can_send(bsp_can_e e, uint32_t id, uint8_t *s) {
    BSP_ASSERT(handle[e]);
    FDCAN_TxHeaderTypeDef header = {
        .Identifier = id,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = FDCAN_DLC_BYTES_8,
        .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
        .BitRateSwitch = FDCAN_BRS_OFF,
        .FDFormat = FDCAN_CLASSIC_CAN,
        .TxEventFifoControl = FDCAN_STORE_TX_EVENTS,
        .MessageMarker = 0x01
    };
    while(handle[e]->Instance->TXFQS & FDCAN_TXFQS_TFQF) __NOP();
    HAL_FDCAN_AddMessageToTxFifoQ(handle[e], &header, s);
}

static uint32_t get_data_length(uint8_t l) {
    if(l <= 8) return l;
    if(l <= 12) return FDCAN_DLC_BYTES_12;
    if(l <= 16) return FDCAN_DLC_BYTES_16;
    if(l <= 20) return FDCAN_DLC_BYTES_20;
    if(l <= 24) return FDCAN_DLC_BYTES_24;
    if(l <= 32) return FDCAN_DLC_BYTES_32;
    if(l <= 48) return FDCAN_DLC_BYTES_48;
    if(l <= 64) return FDCAN_DLC_BYTES_64;
    BSP_ASSERT(0); return 0;
}

void bsp_can_fd_send(bsp_can_e e, uint32_t id, uint8_t *s, uint8_t l) {
    BSP_ASSERT(handle[e]);
    FDCAN_TxHeaderTypeDef header = {
        .Identifier = id,
        .IdType = id > 0x7ff ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID,
        .TxFrameType = FDCAN_DATA_FRAME,
        .DataLength = get_data_length(l),
        .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
        .BitRateSwitch = FDCAN_BRS_ON,
        .FDFormat = FDCAN_FD_CAN,
        .TxEventFifoControl = FDCAN_STORE_TX_EVENTS,
        .MessageMarker = 0x01
    };
    while(handle[e]->Instance->TXFQS & FDCAN_TXFQS_TFQF) __NOP();
    HAL_FDCAN_AddMessageToTxFifoQ(handle[e], &header, s);
}

void bsp_can_rx_sol(bsp_can_e e, uint32_t fifo) {
    bsp_can_msg_t msg = { .port = e };
    while(HAL_FDCAN_GetRxFifoFillLevel(handle[e], fifo)) {
        if(HAL_FDCAN_GetRxMessage(handle[e], fifo, &msg.header, msg.data) != HAL_OK) break;
        if(msg.header.FDFormat == FDCAN_CLASSIC_CAN || msg.header.FDFormat == FDCAN_FD_CAN) {
            for(uint8_t i = 0; i < cnt[e]; i++) {
                if(rx_id[e][i] == msg.header.Identifier) {
                    BSP_ASSERT(callback[e][i] != NULL);
                    callback[e][i](&msg);
                }
            }
        }
    }
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *h, uint32_t RxFifo0ITs) {
    UNUSED(RxFifo0ITs);
    for(uint8_t i = 0; i < (uint8_t) E_CAN_END_DONT_REMOVE; i++) {
        if(handle[i] == h) bsp_can_rx_sol(i, FDCAN_RX_FIFO0);
    }
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *h, uint32_t RxFifo1ITs) {
    UNUSED(RxFifo1ITs);
    for(uint8_t i = 0; i < (uint8_t) E_CAN_END_DONT_REMOVE; i++) {
        if(handle[i] == h) bsp_can_rx_sol(i, FDCAN_RX_FIFO1);
    }
}