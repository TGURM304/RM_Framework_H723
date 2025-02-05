//
// Created by fish on 2025/1/3.
//

#pragma once

#include <string>

class MotorController;

namespace Controller {
	class Base {
	public:
		std::string type;
		virtual ~Base() = default;
		virtual float update(float current, float target) = 0;
		virtual float update(const MotorController *motor, float target) = 0;
	};
}
