//
// Created by fish on 2025/1/3.
//

#pragma once
#include "controller_base.h"
#include "ctrl_pid.h"
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
		MotorBasePID(uint8_t mode, std::unique_ptr <PID> speed, std::unique_ptr <PID> angle, bool use_power_controller = false)
			: mode_(mode), speed_(std::move(speed)), angle_(std::move(angle)), use_power_controller_(use_power_controller)
		{
			type = "MotorBasePID";
			if(use_power_controller) {

			}
		}
		void clear() const;
		float update(float current, float target) override { return 0; };
		float update(const MotorController *motor, float target) override;
	private:
		uint8_t mode_ = 0;
		bool use_power_controller_ = false;
		std::unique_ptr <PID> speed_, angle_;
	};
}
