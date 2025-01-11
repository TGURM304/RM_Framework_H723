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

void MotorController::update(double target) {
	// Offline Detect
	if(bsp_time_get_ms() - motor_->status.last_online_time > 500) {
		// 500ms
		relax();
		error_code |= APP_MOTOR_ERROR_TIMEOUT;
		return;
	}

	// Relax Mode
	if(relaxed_) return;

	for(auto &[fn, controller] : pipeline_) {
		target = controller->update(fn(), target);
	}
	motor_->update(output = target);
}

void MotorController::add_controller(std::function <double()> fn, std::unique_ptr <Controller::Base> controller) {
	pipeline_.emplace_back(fn, std::move(controller));
}