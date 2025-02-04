//
// Created by fish on 2025/1/3.
//

#pragma once

#include <utility>
#include <vector>
#include <memory>
#include <functional>

#include "controller.h"
#include "dev_motor.h"

#define APP_MOTOR_ERROR_TIMEOUT 0b00000001
#define APP_MOTOR_ERROR_STALL   0b00000010

class MotorController {
public:
	MotorController() = default;
	explicit MotorController(std::unique_ptr <Motor::Base> motor) : motor_(std::move(motor)) {}

	void init() const;
	void relax();
	void activate();
	void update(double target);
	void add_controller(std::unique_ptr <Controller::Base> controller);
	void add_controller(const std::function <float(const MotorController *)>& fn, std::unique_ptr <Controller::Base> controller);

	const MotorStatus *device() const { return &motor_->status; }

	unsigned char error_code = 0;
	float output = 0;
	float speed = 0, angle = 0, current = 0, torque = 0;	// Motor Status
	/* 扩展功能 */
	float encoder_zero = 0;
	bool use_extend_angle = false;							// 使用扩展角度（总角度）
	bool use_degree_angle = false;							// 使用角度制
private:
	bool relaxed_ = false;
	int err_stall_count_ = 0;
	float lst_angle_ = 0, cur_angle_ = 0;
	std::unique_ptr <Motor::Base> motor_;
	std::vector <std::tuple<std::function<float(const MotorController *)>, std::unique_ptr<Controller::Base>>> pipeline_;
};
