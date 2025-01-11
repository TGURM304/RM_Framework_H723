//
// Created by fish on 2025/1/3.
//

#pragma once
#include "controller.h"

namespace Controller {
	class PID : public Base {
	public:
		PID() = default;
		~PID() override = default;
		PID(double Kp, double Ki, double Kd, double out_limit, double iout_limit)
			: Kp_(Kp), Ki_(Ki), Kd_(Kd), out_limit_(out_limit), iout_limit_(iout_limit)
		{
			type = "PID";
		}
		void clear();
		double update(double current, double target) override;
	private:
		double Kp_, Ki_, Kd_, out_limit_, iout_limit_;
		double err[3]{}, p_out = 0, i_out = 0, d_out = 0, out = 0;
	};
}
