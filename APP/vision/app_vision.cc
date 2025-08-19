//
// Created by fish on 2024/12/18.
//

#include "app_vision.h"

#include <algorithm>
#include <cstring>

#include "app_ins.h"
#include "bsp_uart.h"
#include "alg_crc.h"
#include "bsp_def.h"

using namespace vision;

static RecvPacket rx_packet;
/**
 * @brief  串口接收回调函数，处理视觉交互串口传过来的数据
 *
 */
void uart_rx_callback(bsp_uart_e e, uint8_t *s, uint16_t l) {
    if(l < sizeof rx_packet) return;
    std::copy_n(s, sizeof rx_packet, reinterpret_cast <uint8_t *> (&rx_packet));
}

/**
 * @brief  视觉模块初始化函数，启用视觉交互串口
 *
 */
void vision::init() {
    bsp_uart_init(E_UART_VISION, &huart10);
    bsp_uart_set_callback(E_UART_VISION, uart_rx_callback);
}
/**
 * @brief  视觉模块传输数据指针
 *
 * @return  接收到的数据（内容参照结构体 RecvPacket）
 */
RecvPacket *vision::recv() {
    return &rx_packet;
}

static auto ins = app_ins_data();

/**
 * @brief  视觉模块发送控制命令函数
 *  @param detect_color 视觉模块要识别的颜色
 *  @param reset_tracker 是否重置目标跟踪器
 *
 */
void vision::send(uint8_t detect_color, bool reset_tracker) {
    SendPacket pkg = {
        .detect_color = detect_color,
        .reset_tracker = reset_tracker,
        .reserved = 0,
        /* 世界坐标系下云台姿态  */
        .roll = static_cast <float> (ins->roll / 180 * M_PI),
        .pitch = static_cast <float> (ins->pitch / 180 * M_PI),
        .yaw = static_cast <float> (ins->yaw / 180 * M_PI),
        /* 当前云台瞄准的位置，用于发布可视化 Marker */
        .aim_x = 0,
        .aim_y = 0,
        .aim_z = 0,
        .checksum = 0
    };

    // TODO: CRC 未测试
    CRC16::append(pkg);
    bsp_uart_send(E_UART_VISION, reinterpret_cast <uint8_t *> (&pkg), sizeof pkg);
}