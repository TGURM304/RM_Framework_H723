//
// Created by fish on 2025/1/3.
//

#pragma once
#include "controller.h"
#include "ctrl_pid.h"

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
		MotorBasePID(uint8_t mode, std::unique_ptr <PID> speed, std::unique_ptr <PID> angle)
			: mode_(mode), speed_(std::move(speed)), angle_(std::move(angle))
		{
			type = "MotorBasePID";
		}
		void clear() const;
		float update(float current, float target) override { return 0; };
		float update(const MotorController *motor, float target) override;
	private:
		uint8_t mode_ = 0;
		std::unique_ptr <PID> speed_, angle_;
	};
}
