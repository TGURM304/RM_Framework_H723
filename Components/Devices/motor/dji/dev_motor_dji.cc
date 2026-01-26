//
// Created by fish on 2024/11/15.
//

#include "dev_motor_dji.h"

#include <cstdio>

#include "bsp_can.h"
#include "bsp_def.h"
#include "bsp_time.h"
#include <cstring>

#include "dev_cap.h"

using namespace Motor;

static bool inited = false;
static constexpr uint16_t ctrl_id_map[] = { 0x2ff, 0x1ff, 0x2fe, 0x1fe, 0x200 };
static constexpr size_t ctrl_id_map_size = sizeof(ctrl_id_map) / sizeof(uint16_t);
static uint8_t id_trans(uint16_t x) {
    for(uint8_t i = 0; i < ctrl_id_map_size; i++) if(ctrl_id_map[i] == x) return i;
    BSP_ASSERT(false); return 0;
}

// Device Class Ptr & Device Cnt (以 can_e 分类，保证一条 can 总线上的设备 id 不冲突)
static DJIMotor* device_ptr[BSP_CAN_ENUM_SIZE][DJI_MOTOR_LIMIT];
static uint8_t device_cnt[BSP_CAN_ENUM_SIZE];
static uint8_t can_tx_buf[BSP_CAN_ENUM_SIZE][ctrl_id_map_size + 1][8];
static bool ctrl_id_used[BSP_CAN_ENUM_SIZE][ctrl_id_map_size + 1];

DJIMotor::DJIMotor(const char *name, const Model &model, const Param &param) : model_(model), param_(param), output_(0) {
    BSP_ASSERT(model == GM6020 or model == M3508 or model == M2006);
    BSP_ASSERT(0 <= param.port and param.port < BSP_CAN_ENUM_SIZE);

    // Init Motor Param
    strcpy(name_, name);
    if(model == GM6020) {
        BSP_ASSERT(1 <= param.id and param.id <= 7);
        BSP_ASSERT(param.mode == CURRENT or param.mode == VOLTAGE);
        if(param.mode == VOLTAGE) {
            ctrl_id = param.id < 5 ? 0x1ff : 0x2ff;
        }
        if(param.mode == CURRENT) {
            ctrl_id = param.id < 5 ? 0x1fe : 0x2fe;
        }
        feedback_id = 0x204 + param.id;
    }
    if(model == M3508 || model == M2006) {
        BSP_ASSERT(1 <= param.id and param.id <= 8);
        // WARNING: M3508 (C620) 仅支持电流控制模式，请不要填写 VOLTAGE 迷惑自己，为了保证一致性，这将导致下面的 ASSERT 死循环。
        BSP_ASSERT(param.mode == CURRENT);
        ctrl_id = param.id < 5 ? 0x200 : 0x1ff;
        feedback_id = 0x200 + param.id;
    }

    device_ptr[param.port][device_cnt[param.port] ++] = this;
    ctrl_id_used[param.port][id_trans(ctrl_id)] = true;
}


TaskHandle_t task_handle;
void dev_dji_motor_task(void *arg);

void DJIMotor::init() {
    if(!inited) {
        inited = true;
        xTaskCreate(dev_dji_motor_task, "dji_motor", 512, nullptr, 5, &task_handle);
    }
    // Can Register
    bsp_can_set_callback(param_.port, feedback_id, dev_dji_motor_can_callback);
    // Enable the motor
    enable();
}

void DJIMotor::update(float output) {
    if(!enabled_) return;
    output_ = output;
    uint8_t cid = id_trans(ctrl_id), mid = param_.id < 5 ? param_.id : param_.id - 4;
    can_tx_buf[param_.port][cid][(mid - 1) << 1] = static_cast <int16_t> (output) >> 8;
    can_tx_buf[param_.port][cid][(mid - 1) << 1 | 1] = static_cast <int16_t> (output) & 0xff;
}

void DJIMotor::clear() {
    update(0);
    status.angle = status.current = status.speed = status.temperature = 0;
}

void DJIMotor::enable() {
    if(enabled_) return;
    enabled_ = true;
}

void DJIMotor::disable() {
    if(!enabled_) return;
    clear();
    enabled_ = false;
}

void dev_dji_motor_can_callback(bsp_can_msg_t *msg) {
    if(!device_cnt[msg->port]) return;

    DJIMotor *p = nullptr;
    for(uint8_t i = 0; i < device_cnt[msg->port]; i++) {
        if(device_ptr[msg->port][i]->feedback_id == msg->header.Identifier) {
            p = device_ptr[msg->port][i];
            break;
        }
    }
    if(p == nullptr) return;

    uint8_t *s = msg->data;

    p->feedback_.angle = static_cast <int16_t> (s[0] << 8 | s[1]);
    p->feedback_.speed = static_cast <int16_t> (s[2] << 8 | s[3]);
    p->feedback_.current = static_cast <int16_t> (s[4] << 8 | s[5]);
    p->feedback_.temp = s[6];

    p->status.angle = p->feedback_.angle;
    p->status.speed = p->feedback_.speed;
    p->status.current = p->feedback_.current;
    p->status.temperature = p->feedback_.temp;

    p->status.last_online_time = bsp_time_get_ms();
}

void dev_dji_motor_task(void *arg) {
    UNUSED(arg);
    while(!inited) osDelay(10);
    while(true) {
        for(uint8_t i = 0; i < BSP_CAN_ENUM_SIZE; i++) {
            if(!device_cnt[i]) continue;
            // Send Control Message
            for(uint8_t j = 0; j < ctrl_id_map_size; j++) {
                if(ctrl_id_used[i][j]) {
                    bsp_can_send(static_cast <bsp_can_e> (i), ctrl_id_map[j], can_tx_buf[i][j]);
                }
            }
        }
        // For freertos tasks, a delay of 1ms is important.
        osDelay(1);
    }
}