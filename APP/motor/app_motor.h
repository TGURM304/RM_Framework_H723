//
// Created by fish on 2025/1/3.
//

#pragma once

#include <utility>
#include <vector>
#include <memory>
#include <functional>

#include "app_motor_base_controller.h"
#include "controller.h"
#include "dev_motor.h"

#define APP_MOTOR_ERROR_TIMEOUT 0b00000001

class MotorController {
public:
	MotorController() = default;
	explicit MotorController(std::unique_ptr <Motor::Base> motor) : motor_(std::move(motor)) {}

	void init() const;
	void relax();
	void activate();
	void update(double target);
	void add_controller(const std::function <double(const MotorController *)>& fn, std::unique_ptr <Controller::Base> controller);

	const MotorStatus *status() const { return &motor_->status; }

	double output = 0;
	unsigned char error_code = 0;
private:
	bool relaxed_ = false;
	std::unique_ptr <Motor::Base> motor_;
	std::vector <std::tuple<std::function<double(const MotorController *)>, std::unique_ptr<Controller::Base>>> pipeline_;
};
