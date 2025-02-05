//
// Created by fish on 2024/11/15.
//

#pragma once

#include "motor_base.h"
#include "bsp_can.h"
#include <cstdint>

// 单个 Can 总线挂载电机数限制
#define DJI_MOTOR_LIMIT 4

/*
 *  DJI Motor Driver
 *  GM6020:
 *      Feedback id: 0x205 - 0x20B
 *      Control id: 0x1FF/0x2FF (VOLTAGE), 0x1FE/0x2FE (CURRENT)
 *      VOLTAGE Range: -25000 ~ 25000
 *      CURRENT Range: -16384 ~ 16384
 *      Angle Range: 0 ~ 8191
 *      Speed Unit: rpm
 *  M3508 (C620):
 *      Feedback id: 0x201 - 0x208
 *      Control id: 0x200/0x1FF (CURRENT)
 *      CURRENT Range: -16384 ~ 16384 (-20A ~ 20A)
 *      Angle Range: 0 ~ 8191
 *      Speed Unit: rpm
 */

namespace Motor {
	class DJIMotor : public Base {
	public:
		DJIMotor() = default;
		~DJIMotor() override = default;

		enum ControlMode {
			VOLTAGE, CURRENT
		};

		enum Model {
			MODEL_DEFAULT, GM6020, M3508, M2006
		};

		struct Param {
			uint8_t id;
			bsp_can_e port;
			ControlMode mode;
		};

		struct Feedback {
			int16_t angle, speed, current;
			uint8_t temp;
		};

		DJIMotor(const char *name, const Model &model, const Param &param);

		void init() override;
		void clear();

		void enable() override;
		void disable() override;
		void update(float output) override;

		float output_ = 0;
		uint16_t ctrl_id = 0, feedback_id = 0;
		Feedback feedback_ = Feedback();
		char name_[128] = { 0 };
	private:
		Model model_ = MODEL_DEFAULT;
		Param param_ = Param();
	};
}

#ifdef __cplusplus
extern "C" {
#endif

void dev_dji_motor_can_callback(bsp_can_msg_t *msg);
void dev_dji_motor_task(void *arg);

#ifdef __cplusplus
}
#endif