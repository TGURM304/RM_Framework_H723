//
// Created by fish on 2025/1/3.
//

#pragma once

#include <string>

namespace Controller {
	class Base {
	public:
		std::string type;
		virtual ~Base() = default;
		virtual double update(double current, double target) = 0;
	};
}
