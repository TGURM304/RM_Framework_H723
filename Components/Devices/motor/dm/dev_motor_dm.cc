//
// Created by fish on 2024/12/5.
//

#include "dev_motor_dm.h"

#include <cstring>
#include <numeric>

#include "BMI088reg.h"
#include "bsp_def.h"

using namespace Motor;

static DMMotor *device_ptr[BSP_CAN_ENUM_SIZE];
uint8_t device_cnt[BSP_CAN_ENUM_SIZE];

DMMotor::DMMotor(const char *name, const Model &model, const Param &param) : model_(model), param_(param){
    BSP_ASSERT(model == J4310 or model == J8009P);
    BSP_ASSERT(0 <= param.port and param.port < BSP_CAN_ENUM_SIZE);

    if(model == J4310 or model == J8009P) {
        if(param.mode == MIT) {
            ctrl_id = param.slave_id;
        }
        if(param.mode == POSITION_SPEED) {
            ctrl_id = 0x100 + param.slave_id;
        }
        if(param.mode == SPEED) {
            ctrl_id = 0x200 + param.slave_id;
        }
    }

    feedback_id = param.master_id;
    device_ptr[device_cnt[param.port] ++] = this;
}

void DMMotor::init() {
    bsp_can_set_callback(param_.port, feedback_id, dev_dm_motor_can_callback);
}

void DMMotor::reset() {
    uint8_t msg[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfb };
    bsp_can_send(param_.port, ctrl_id, msg);
}

void DMMotor::enable() {
    // if(enabled_) return;
    uint8_t msg[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc };
    bsp_can_send(param_.port, ctrl_id, msg);
    enabled_ = true;
}

void DMMotor::disable() {
    // if(!enabled_) return;
    uint8_t msg[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd };
    bsp_can_send(param_.port, ctrl_id, msg);
    enabled_ = false;
}

float uint_to_float(int x_int, float x_min, float x_max, int bits) {
    float span = x_max - x_min, offset = x_min;
    return static_cast <float> (x_int) * span / static_cast <float> ((1 << bits) - 1) + offset;
}

int float_to_uint(float x, float x_min, float x_max, int bits) {
    float span = x_max - x_min, offset = x_min;
    return static_cast <int> ((x - offset) * (static_cast <float> ((1 << bits) - 1)) / span);
}

// SPEED
void DMMotor::control(float speed) const {
    BSP_ASSERT(param_.mode == SPEED);
    if(!enabled_) return;
    uint8_t msg[8] = { 0 };
    memcpy(msg, &speed, sizeof speed);
    bsp_can_send(param_.port, ctrl_id, msg);
}

// POSITION_SPEED
void DMMotor::control(float position, float speed) const {
    BSP_ASSERT(param_.mode == POSITION_SPEED);
    if(!enabled_) return;
    uint8_t msg[8] = { 0 };
    memcpy(msg, &position, sizeof position);
    memcpy(msg + sizeof position, &speed, sizeof speed);
    bsp_can_send(param_.port, ctrl_id, msg);
}

// MIT
void DMMotor::control(float position, float speed, float Kp, float Kd, float torque) const {
    BSP_ASSERT(param_.mode == MIT);
    if(!enabled_) return;

    uint16_t P_des = float_to_uint(position, -param_.p_max, param_.p_max, 16),
             V_des = float_to_uint(speed, -param_.v_max, param_.v_max, 12),
             Kp_ = float_to_uint(Kp, 0, 500, 12),
             Kd_ = float_to_uint(Kd, 0, 5, 12),
             T_ff = float_to_uint(torque, -param_.t_max, param_.t_max, 12);

    uint8_t msg[8] = { 0 };
    msg[0] = P_des >> 8;
    msg[1] = P_des & 0xff;
    msg[2] = V_des >> 4;
    msg[3] = (V_des & 0xf) << 4 | (Kp_ >> 8);
    msg[4] = Kp_ & 0xff;
    msg[5] = Kd_ >> 4;
    msg[6] = (Kd_ & 0xf) << 4 | (T_ff >> 8);
    msg[7] = T_ff & 0xff;
    bsp_can_send(param_.port, ctrl_id, msg);
}

DMMotor::Param *DMMotor::get_param() {
    return &param_;
}

void DMMotor::update(float output) {
    // Error
    ;
}


void dev_dm_motor_can_callback(bsp_can_msg_t* msg) {
    if(!device_cnt[msg->port]) return;

    DMMotor *p = nullptr;
    for(uint8_t i = 0; i < device_cnt[msg->port]; i++) {
        if(msg->header.Identifier == device_ptr[i]->feedback_id) {
            p = device_ptr[i];
            break;
        }
    }

    if(p == nullptr) return;

    const auto s = msg->data;
    p->feedback_.err = s[0] >> 4;
    p->feedback_.id = s[0] & 0xf;
    p->feedback_.pos = s[1] << 8 | s[2];
    p->feedback_.vel = s[3] << 4 | (s[4] >> 4);
    p->feedback_.t = (s[4] & 0xf) << 8 | s[5];
    p->feedback_.t_mos = s[6];
    p->feedback_.t_rotor = s[7];

    const auto para = p->get_param();
    p->status.pos = uint_to_float(p->feedback_.pos, -para->p_max, para->p_max, 16);
    p->status.vel = uint_to_float(p->feedback_.vel, -para->v_max, para->v_max, 12);
    p->status.torque = uint_to_float(p->feedback_.t, 0, para->t_max, 12);
    p->status.err = p->feedback_.err;
    p->status.t_mos = p->feedback_.t_mos;
    p->status.t_rotor = p->feedback_.t_rotor;
}
