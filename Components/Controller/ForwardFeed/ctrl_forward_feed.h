//
// Created by fish on 2025/1/3.
//

#pragma once

#include "controller_base.h"

namespace Controller {
	class ForwardFeed : public Base {
	public:
		ForwardFeed() = default;
		~ForwardFeed() override = default;

		void clear() override { };
		float update(float current, float target) override;
		float update(const MotorController *motor, float target) override { return 0; }
	};
}
