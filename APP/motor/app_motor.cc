//
// Created by fish on 2025/1/3.
//

#include "app_motor.h"

#include "bsp_time.h"
/**
 * @brief 初始化电机控制器
 *
 * 调用底层 motor_ 的 init() 函数完成初始化。
 */
void MotorController::init() const {
    motor_->init();
}

/**
 * @brief 清空控制器管道
 *
 * 遍历 pipeline_ 中的所有控制器，依次调用其 clear() 方法。
 */
void MotorController::clear() {
    for(auto &[_, controller] : pipeline_) {
        controller->clear();
    }
}

/**
 * @brief 使电机进入松弛（禁用）状态
 *
 * @param force 是否强制进入松弛状态
 *
 * - 如果已处于松弛状态，则直接返回。
 * - 会清空控制器、关闭电机使能，并将 relaxed_ 标记为 true。
 *
 * @note  请将relax写在显眼的位置，不然可能会遗忘导致需要debug
 */
void MotorController::relax(bool force) {
	force_relaxed_ |= force;
	if(relaxed_) return;
	clear();
	motor_->disable();
	relaxed_ = true;
}

/**
 * @brief 激活电机（恢复工作状态）
 *
 * @param force 是否强制激活
 *
 * - 如果电机已处于激活状态，直接返回。
 * - 如果之前是强制松弛，且本次未强制激活，则返回。
 * - 否则重新开启电机使能，并清除 relaxed_ 和 force_relaxed_ 标记。
 */
void MotorController::activate(bool force) {
	if(!relaxed_) return;
	if(force_relaxed_ and !force) return;
	motor_->enable();
	relaxed_ = force_relaxed_ = false;
}

/**
 * @brief 将编码器数值转换为角度值
 *
 * @param x    当前编码器读数 [0, 8192)
 * @param zero 编码器零点偏移
 * @return 转换后的角度值 [0, 360)
 */
static float encoder2deg(float x, float zero) {
    x -= zero;
    if(x < 0) x += 8192;
    return x * 360 / 8192;
}

/**
 * @brief 计算目标角度与当前角度的差值（避免越界）
 *
 * @param full    角度范围（如 360° 或 8192 编码器刻度）
 * @param current 当前角度
 * @param target  目标角度
 * @return 最小化后的角度差（可能为负值）
 */
static float calc_delta(float full, float current, float target) {
    float dt = target - current;
    if(2 * dt >  full) dt -= full;
    if(2 * dt < -full) dt += full;
    return dt;
}

/**
 * @brief 更新电机状态并执行控制逻辑
 *
 * @param target 期望目标值（位置/角度/速度等）
 *
 * 功能：
 * - 检查电机是否超时离线，若超过 500ms 未在线则进入松弛状态并设置错误码。
 * - 执行堵转检测（stall detect），根据电流阈值和时间阈值判断是否堵转。
 * - 更新当前速度、角度、电流、扭矩等状态量。
 * - 依次执行 pipeline_ 中的控制器，计算输出控制量并传递给电机。
 *
 * @note 如果处于松弛模式或存在错误码，则直接返回不更新。
 */
void MotorController::update(double target) {
	target_ = target;
	// Offline Detect
	if(bsp_time_get_ms() - motor_->status.last_online_time > 500) {
		// 500ms
		relax(false);
		error_code |= APP_MOTOR_ERROR_TIMEOUT;
		return;
	}

	if(error_code & APP_MOTOR_ERROR_TIMEOUT)
		activate(false), error_code ^= APP_MOTOR_ERROR_TIMEOUT;

	if(use_stall_detect) {
	    if(error_code & APP_MOTOR_ERROR_STALL) {
	        // Compare absolute current against threshold to detect stall clearance
	        if(std::abs(motor_->status.current) < 1000) {
	            if(++err_stall_count_ == 5 * stall_detector_time_threshold) {
	                activate(false), error_code ^= APP_MOTOR_ERROR_STALL;
	                err_stall_count_ = 0;
	            }
	        } else {
				err_stall_count_ = 0;
				return;
			}
		} else {
			if(std::abs(motor_->status.current) > stall_detector_current_threshold) {
				if(++err_stall_count_ == stall_detector_time_threshold) {
					relax(false), error_code ^= APP_MOTOR_ERROR_STALL;
					err_stall_count_ = 0;
					return;
				}
			} else {
				err_stall_count_ = 0;
			}
		}
	}

	// // Relax Mode or Error Mode
	// if(relaxed_ || error_code) return;

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

	// Relax Mode or Error Mode
	if(relaxed_ || error_code) return;

	auto result = static_cast <float> (target);

	for(auto &[fn, controller] : pipeline_) {
		if(fn == nullptr)
			result = controller->update(this, result);
		else
			result = controller->update(fn(this), result);
	}

	motor_->update(output = result);
}

/**
 * @brief 向控制管道添加一个控制器
 *
 * @param controller 控制器实例（智能指针）
 *
 * 控制器会依次被调用，用于处理和修正控制量。
 */
void MotorController::add_controller(std::unique_ptr <Controller::Base> controller) {
	pipeline_.emplace_back(nullptr, std::move(controller));
}

/**
 * @brief 向控制管道添加一个带函数映射的控制器
 *
 * @param fn         函数对象，输入 MotorController 指针，返回 float 值
 * @param controller 控制器实例（智能指针）
 *
 * 该函数可为控制器提供额外的输入信号，支持更复杂的控制逻辑。
 */
void MotorController::add_controller(const std::function <float(const MotorController *)>& fn, std::unique_ptr <Controller::Base> controller) {
    pipeline_.emplace_back(fn, std::move(controller));
}
