//
// Created by fish on 2025/1/3.
//

#pragma once
#include "controller_base.h"
#include "ctrl_pid.h"

#include <cstdint>
#include <memory>

namespace Controller {
	class MotorBasePID : public Base {
	public:
		typedef enum {
			PID_NONE  = 0b00,
			PID_SPEED = 0b01,
			PID_ANGLE = 0b10
		} Mode;
		MotorBasePID() = default;
		~MotorBasePID() override = default;
		MotorBasePID(uint8_t mode, std::unique_ptr <PID> speed, std::unique_ptr <PID> angle, bool use_micro_arc = false)
			: mode_(mode), speed_(std::move(speed)), angle_(std::move(angle)), use_micro_arc_(use_micro_arc)
		{
			type = "MotorBasePID";
		}
		void clear() override;
		float update(float current, float target) override { return 0; };
		float update(const MotorController *motor, float target) override;
	private:
		uint8_t mode_ = 0;
		bool use_micro_arc_ = false;
		std::unique_ptr <PID> speed_, angle_;
	};
}
