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

OS::Task can_msg_task_;
OS::Queue <app_msg_can_t> can_msg_q_(50);

void can_msg_task(void *args) {
    app_msg_can_t msg {};
    while(true) {
        if(can_msg_q_.size()) {
            can_msg_q_.receive(msg);
            bsp_can_send(msg.port, msg.id, msg.data.begin());
        }
        OS::Task::SleepMilliseconds(1);
    }
}

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

std::function<void(uint8_t*, uint16_t)> can_recv_callback = nullptr;
uint8_t can_recv_buf[512], can_recv_sz = 0, can_recv_tot_sz = 0;

void app_msg_can_recv(bsp_can_msg_t *msg) {
    if(can_recv_sz) {
        auto len = std::min(can_recv_tot_sz - can_recv_sz, 8);
        std::copy_n(msg->data, len, can_recv_buf + can_recv_sz);
        can_recv_sz += len;
    }
    else if(msg->data[0] == 0xa5 and msg->data[1] == 0x5a) {
        can_recv_tot_sz = msg->data[2] + 4; // 帧头 2byte + CRC8 1byte
        auto len = std::min(can_recv_tot_sz - can_recv_sz, 8);
        std::copy_n(msg->data, len, can_recv_buf + can_recv_sz);
        can_recv_sz = std::min(can_recv_tot_sz, static_cast <uint8_t> (8));
    }
    if(can_recv_sz == can_recv_tot_sz) {
        auto crc8 = CRC8::calc(can_recv_buf, can_recv_tot_sz - 1);
        if(crc8 == can_recv_buf[can_recv_tot_sz - 1]) {
            if(can_recv_callback != nullptr) {
                can_recv_callback(can_recv_buf + 3, can_recv_tot_sz - 4);
            }
        } else {
            BSP_ASSERT(0);
        }
    }
}

void app_msg_can_set_callback(bsp_can_e e, uint32_t id, std::function<void(uint8_t*, uint16_t)> callback) {
    bsp_can_set_callback(e, id, app_msg_can_recv);
    can_recv_callback = std::move(callback);
}