//
// Created by fish on 2024/11/17.
//

#include "app_msg.h"

#include "alg_crc.h"
#include "bsp_can.h"
#include "sys_queue.h"
#include "sys_task.h"

#include <algorithm>
#include <functional>
#include <utility>

OS::Task can_msg_task_;                      // CAN 发送任务
OS::Queue <app_msg_can_t> can_msg_q_(50);    // CAN 发送消息队列，容量 50

/**
 * @brief  CAN消息发送任务
 *
 * -从队列中取出待发送的 CAN 消息，并调用BSP层的发送函数。
 * -每毫秒最多发送 MSG_CAN_LIMIT_PER_MILLISECOND 条消息，防止过载。
 *
 */
void can_msg_task(void *args) {
    app_msg_can_t msg {};
    while(true) {
        int count = 0;
        while(can_msg_q_.size() and count++ < MSG_CAN_LIMIT_PER_MILLISECOND) {
            can_msg_q_.receive(msg);
            bsp_can_send(msg.port, msg.id, msg.data.begin());
            OS::Task::Yield();
        }
        OS::Task::SleepMilliseconds(1);
    }
}

/**
 * @brief 向 CAN 发送队列中添加一条消息
 *
 * -将数据封装为 app_msg_can_t 并加入发送队列，
 * -如果发送任务尚未创建，则自动创建。
 *
 * @param e   CAN 端口
 * @param id  CAN 消息 ID
 * @param s   待发送的数据指针（长度 <= 8 字节）
 */
void app_msg_can_send(bsp_can_e e, uint32_t id, uint8_t *s) {
    std::array <uint8_t, 8> data({});
    std::copy_n(s, data.size(), data.begin());
    can_msg_q_.send({
        .port = e,
        .id = id,
        .data = data
    });
    if(can_msg_task_.handle_ == nullptr) {
        can_msg_task_.Create(can_msg_task, static_cast <void *> (nullptr), "msg_can", 512, OS::Task::MEDIUM);
    }
}

std::function<void(uint8_t*, uint16_t)> can_recv_callback = nullptr; // 用户定义的 CAN 接收回调
uint8_t can_recv_buf[512];          // 接收缓冲区
uint8_t can_recv_sz = 0;            // 当前已接收数据长度
uint8_t can_recv_tot_sz = 0;        // 总帧长度

/**
 * @brief CAN 消息接收处理函数
 *
 * 根据协议组装多帧 CAN 数据：
 * - 帧头固定为 0xA5 0x5A
 * - 第 3 个字节为数据长度
 * - 帧尾包含 1 字节 CRC8 校验
 *
 * 当一帧完整数据接收完成并通过 CRC8 校验后，调用用户回调函数。
 *
 * @param msg 接收到的 CAN 消息（bsp_can_msg_t）
 */
void app_msg_can_recv(bsp_can_msg_t *msg) {
    if(can_recv_sz) {
        auto len = std::min(can_recv_tot_sz - can_recv_sz, 8);
        std::copy_n(msg->data, len, can_recv_buf + can_recv_sz);
        can_recv_sz += len;
    }
    else if(msg->data[0] == 0xa5 and msg->data[1] == 0x5a) {
        can_recv_tot_sz = msg->data[2] + 4; // 帧头 2B + CRC8 1B + 长度 1B
        auto len = std::min(can_recv_tot_sz - can_recv_sz, 8);
        std::copy_n(msg->data, len, can_recv_buf + can_recv_sz);
        can_recv_sz = std::min(can_recv_tot_sz, static_cast <uint8_t> (8));
    }
    if(can_recv_sz and can_recv_sz == can_recv_tot_sz) {
        auto crc8 = CRC8::calc(can_recv_buf, can_recv_tot_sz - 1);
        if(crc8 == can_recv_buf[can_recv_tot_sz - 1]) {
            if(can_recv_callback != nullptr) {
                can_recv_callback(can_recv_buf + 3, can_recv_tot_sz - 4);
            }
        } else {
            // BSP_ASSERT(0);
        }
        can_recv_sz = can_recv_tot_sz = 0;
    }
}

/**
 * @brief 设置 CAN 接收回调函数
 *
 * -为指定的 CAN 端口和消息 ID 注册接收回调
 * -当接收到完整有效的应用层数据帧时触发
 *
 * @param e        CAN 端口
 * @param id       过滤的 CAN 消息 ID
 * @param callback 用户回调函数，参数为 (数据指针, 数据长度)
 */
void app_msg_can_set_callback(bsp_can_e e, uint32_t id, std::function<void(uint8_t*, uint16_t)> callback) {
    bsp_can_set_callback(e, id, app_msg_can_recv);
    can_recv_callback = std::move(callback);
}
