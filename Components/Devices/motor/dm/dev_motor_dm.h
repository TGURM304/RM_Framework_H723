//
// Created by fish on 2024/12/5.
//

#pragma once

#include "dev_motor.h"
#include "bsp_can.h"
#include <cstdint>

// 单个 Can 总线挂载电机数限制
#define DM_MOTOR_LIMIT 4

/*
 *  DM Motor Driver
 */

namespace Motor {
    class DMMotor : public Base {
    public:
        DMMotor() = default;
        ~DMMotor() override = default;

        enum ControlMode {
            MIT, POSITION_SPEED, SPEED
        };

        enum Model {
            MODEL_DEFAULT, J4310, J8009P
        };

        struct Param {
            uint8_t slave_id, master_id;
            bsp_can_e port;
            ControlMode mode;
            float p_max, v_max, t_max, kp_max, kd_max;
        };

        struct Status {
            uint8_t err;
            float pos, vel, torque, t_mos, t_rotor;
        };

        struct Feedback {
            uint8_t id, err;
            uint16_t pos;
            uint16_t vel;
            uint8_t t, t_mos, t_rotor;
        };

        Status status = { 0 };

        DMMotor(const char *name, const Model &model, const Param &param);

        void init() override;

        // Base Control
        void reset();
        void enable() override;
        void disable() override;
        // MIT Control
        void control(float position, float speed, float Kp, float Kd, float torque) const;
        // Position Speed Control
        void control(float position, float speed) const;
        // Speed Control
        void control(float speed) const;
        Param *get_param();

        // Base Update
        void update(float output) override;

        uint16_t ctrl_id = 0, feedback_id = 0;
        Feedback feedback_ = Feedback();
        char name_[128] = { 0 };
    private:
        Model model_ = MODEL_DEFAULT;
        Param param_ = Param();
        bool enabled = false;
    };
}

#ifdef __cplusplus
extern "C" {
#endif

    void dev_dm_motor_can_callback(bsp_can_msg_t *msg);
    void dev_dm_motor_task(void *arg);

#ifdef __cplusplus
}
#endif