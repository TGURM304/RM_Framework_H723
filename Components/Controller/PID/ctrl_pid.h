//
// Created by fish on 2025/1/3.
//

#pragma once
#include "controller.h"

namespace Controller {
	class PID : public Base {
	public:
		typedef struct {
			double Kp, Ki, Kd, out_limit, iout_limit;
		} pid_param_t;
		PID() = default;
		~PID() override = default;
		PID(double Kp, double Ki, double Kd, double out_limit, double iout_limit)
			: Kp_(Kp), Ki_(Ki), Kd_(Kd), out_limit_(out_limit), iout_limit_(iout_limit)
		{
			type = "PID";
		}
		explicit PID(const pid_param_t& param)
			: Kp_(param.Kp), Ki_(param.Ki), Kd_(param.Kd), out_limit_(param.out_limit), iout_limit_(param.iout_limit)
		{
			type = "PID";
		}
		void clear();
		float update(float current, float target) override;
		float update(const MotorController *motor, float target) override { return 0; }
	private:
		double Kp_, Ki_, Kd_, out_limit_, iout_limit_;
		double err[3]{}, p_out = 0, i_out = 0, d_out = 0, out = 0;
	};
}
