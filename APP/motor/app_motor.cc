//
// Created by fish on 2025/1/3.
//

#include "app_motor.h"

#include "bsp_time.h"

void MotorController::init() const {
	motor_->init();
}

void MotorController::relax() {
	if(relaxed_) return;
	motor_->disable();
	relaxed_ = true;
}

void MotorController::activate() {
	if(!relaxed_) return;
	motor_->enable();
	relaxed_ = false;
}

// Convert encoder val to degree.
// [0, 8192) -> [0, 360)
static float encoder2deg(float x, float zero) {
	x -= zero;
	if(x < 0) x += 8192;
	return x * 360 / 8192;
}

static float calc_delta(float full, float current, float target) {
	float dt = target - current;
	if(2 * dt >  full) dt -= full;
	if(2 * dt < -full) dt += full;
	return dt;
}

void MotorController::update(double target) {
	// Offline Detect
	if(bsp_time_get_ms() - motor_->status.last_online_time > 500) {
		// 500ms
		relax();
		error_code |= APP_MOTOR_ERROR_TIMEOUT;
		return;
	}

	if(error_code & APP_MOTOR_ERROR_TIMEOUT)
		activate(), error_code ^= APP_MOTOR_ERROR_TIMEOUT;

	// Relax Mode or Error Mode
	if(relaxed_ || error_code) return;

	std::tie(speed, cur_angle_, current, torque) = std::make_tuple <double> (
		motor_->status.speed,
		use_degree_angle ? encoder2deg(motor_->status.angle, encoder_zero) : motor_->status.angle,
		motor_->status.current,
		motor_->status.torque
	);

	if(use_extend_angle) {
		if(use_degree_angle)
			angle -= calc_delta(360, cur_angle_, lst_angle_);
		else
			angle -= calc_delta(8192, cur_angle_, lst_angle_);
		lst_angle_ = cur_angle_;
	} else {
		angle = cur_angle_;
	}

	auto result = static_cast <float> (target);

	for(auto &[fn, controller] : pipeline_) {
		if(fn == nullptr)
			result = controller->update(this, result);
		else
			result = controller->update(fn(this), result);
	}

	motor_->update(output = result);
}

void MotorController::add_controller(std::unique_ptr <Controller::Base> controller) {
	pipeline_.emplace_back(nullptr, std::move(controller));
}

void MotorController::add_controller(const std::function <float(const MotorController *)>& fn, std::unique_ptr <Controller::Base> controller) {
	pipeline_.emplace_back(fn, std::move(controller));
}