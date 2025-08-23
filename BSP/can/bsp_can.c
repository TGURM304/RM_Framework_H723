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


/**
 * @brief  CAN 总线初始化
 *
 * 该函数用于初始化指定 CAN 端口，包括：
 * - 保存句柄
 * - 激活 TX FIFO 空中断和 RX FIFO 新消息中断
 * - 配置全局过滤器，丢弃所有未配置的消息
 * - 启动 CAN 外设
 *
 * @param  e  CAN 端口
 * @param  h  FDCAN 外设句柄指针
 */
void bsp_can_init(bsp_can_e e, FDCAN_HandleTypeDef *h) {
    handle[e] = h;
    HAL_FDCAN_ActivateNotification(h, FDCAN_IT_TX_FIFO_EMPTY, 0);
    HAL_FDCAN_ActivateNotification(h, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(h, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
    HAL_FDCAN_ConfigGlobalFilter(h, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_REJECT_REMOTE);
    // 一定要在配置完后 Start，否则若总线上有 CAN 包，设备会 BUSY。
    HAL_FDCAN_Start(h);
}

/**
 * @brief  设置 CAN 接收回调函数
 *
 * 给指定 CAN 端口的指定消息 ID 设置回调函数，当接收到对应 ID 的 CAN 帧时调用
 * - 支持标准 ID（11 位）
 * - 根据 ID 奇偶性决定放入 RX FIFO0 或 FIFO1
 *
 * @param  e   CAN 端口枚举值
 * @param  id  消息 ID
 * @param  f   回调函数指针，接收到消息时执行
 * @return     分配的过滤器索引号
 */
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

/**
 * @brief  发送 CAN 数据帧
 *
 * -将指定 ID 的 8 字节数据帧发送到 CAN 总线。
 * -会等待 TX FIFO 有空闲位置后再发送
 *
 * @param  e   CAN 端口枚举
 * @param  id  消息 ID
 * @param  s   8 字节数据缓冲区
 */
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
/**
 * @brief      根据要发送的数据长度获取 FDCAN 数据长度码(DLC)值
 * @param[in]  l  实际数据长度（字节数）
 * @return     FDCAN DLC 对应的长度值
 *
 * @note
 * - CAN 总线的数据帧在经典 CAN 中最多 8 字节，
 *   FDCAN (CAN FD) 支持 8~64 字节的数据帧。
 * - FDCAN 使用 DLC 字段表示实际数据长度：
 * - 如果传入长度超过 64 字节，会触发断言错误（BSP_ASSERT(0)）。
 *
 */
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
/**
 * @brief      发送一帧 CAN FD 数据
 * @param  e   CAN 端口枚举 (bsp_can_e)
 * @param  id  CAN 消息 ID（标准 11 位或扩展 29 位）
 * @param  s   指向要发送的数据缓冲区
 * @param  l   数据长度（字节数，1~64）
 *
 * @note
 * - 这是 FDCAN (CAN FD) 模式下发送数据的函数，支持 8~64 字节的数据帧。
 *
 * - 数据长度通过 get_data_length(l) 映射成 FDCAN DLC 字段值。
 *
 */
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

/**
 * @brief  CAN 接收处理函数
 *
 * 遍历指定 FIFO 中所有消息，并调用注册的回调函数。
 * - 仅处理经典 CAN 帧 （FDCAN 暂不支持）
 *
 * @param  e     CAN 端口枚举值
 * @param  fifo  FIFO 号（FDCAN_RX_FIFO0 / FDCAN_RX_FIFO1）
 */
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

/**
 * @brief  RX FIFO0 中断回调函数
 *
 * 当 FDCAN RX FIFO0 收到新消息时触发。
 * 会调用 bsp_can_rx_sol 处理接收消息。
 *
 * @param  h           FDCAN 外设句柄
 * @param  RxFifo0ITs  中断状态标志
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *h, uint32_t RxFifo0ITs) {
    UNUSED(RxFifo0ITs);
    for(uint8_t i = 0; i < (uint8_t) E_CAN_END_DONT_REMOVE; i++) {
        if(handle[i] == h) bsp_can_rx_sol(i, FDCAN_RX_FIFO0);
    }
}

/**
 * @brief  RX FIFO1 中断回调函数
 *
 * 当 FDCAN RX FIFO1 收到新消息时触发。
 * 会调用 bsp_can_rx_sol 处理接收消息。
 *
 * @param  h           FDCAN 外设句柄
 * @param  RxFifo1ITs  中断状态标志
 */
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *h, uint32_t RxFifo1ITs) {
    UNUSED(RxFifo1ITs);
    for(uint8_t i = 0; i < (uint8_t) E_CAN_END_DONT_REMOVE; i++) {
        if(handle[i] == h) bsp_can_rx_sol(i, FDCAN_RX_FIFO1);
    }
}