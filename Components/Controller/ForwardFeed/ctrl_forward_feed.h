//
// Created by fish on 2025/1/3.
//

#pragma once

#include "controller.h"

namespace Controller {
	class ForwardFeed : public Base {
	public:
		ForwardFeed() = default;
		~ForwardFeed() override = default;

		double update(double current, double target) override;
	};
}
