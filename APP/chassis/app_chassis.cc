//
// Created by fish on 2024/12/18.
//

#include "app_chassis.h"

#include "app_motor.h"
#include "app_msg.h"
#include "app_sys.h"
#include "bsp_rc.h"
#include "bsp_time.h"
#include "ctrl_motor_base_pid.h"
#include "ctrl_low_pass_filter.h"
#include "dev_motor_dji.h"
#include "sys_task.h"

#ifdef COMPILE_CHASSIS

using namespace Motor;
using namespace Controller;

// 轮
MotorController w_1(std::make_unique <DJIMotor> (
	"w_1",
	DJIMotor::M3508,
	(DJIMotor::Param) { 0x01, E_CAN1, DJIMotor::CURRENT }
));
MotorController w_2(std::make_unique <DJIMotor> (
	"w_2",
	DJIMotor::M3508,
	(DJIMotor::Param) { 0x02, E_CAN1, DJIMotor::CURRENT }
));
MotorController w_3(std::make_unique <DJIMotor> (
	"w_3",
	DJIMotor::M3508,
	(DJIMotor::Param) { 0x03, E_CAN1, DJIMotor::CURRENT }
));
MotorController w_4(std::make_unique <DJIMotor> (
	"w_4",
	DJIMotor::M3508,
	(DJIMotor::Param) { 0x04, E_CAN1, DJIMotor::CURRENT }
));
// 舵
MotorController s_1(std::make_unique <DJIMotor> (
	"s_1",
	DJIMotor::GM6020,
	(DJIMotor::Param) { 0x01, E_CAN2, DJIMotor::CURRENT }
));
MotorController s_2(std::make_unique <DJIMotor> (
	"s_2",
	DJIMotor::GM6020,
	(DJIMotor::Param) { 0x02, E_CAN2, DJIMotor::CURRENT }
));
MotorController s_3(std::make_unique <DJIMotor> (
	"s_3",
	DJIMotor::GM6020,
	(DJIMotor::Param) { 0x03, E_CAN2, DJIMotor::CURRENT }
));
MotorController s_4(std::make_unique <DJIMotor> (
	"s_4",
	DJIMotor::GM6020,
	(DJIMotor::Param) { 0x04, E_CAN2, DJIMotor::CURRENT }
));

struct SW {
	MotorController *servo = nullptr, *wheel = nullptr;
	bool reversed = false;
	double vx_ = 0, vy_ = 0, angle_ = 0;
	void update(double vx, double vy) {
		vx *= -1;
		auto angle = std::atan2(vy, vx);
		if(std::min(std::abs(angle - angle_), 2 * M_PI - std::abs(angle - angle_)) > M_PI / 2) {
			reversed ^= 1;
		}
		auto cur_angle = static_cast <float> (angle < 0 ? 2 * M_PI + angle : angle), cur_speed = static_cast <float> (std::sqrt(vx * vx + vy * vy));
		cur_angle = static_cast <float> (cur_angle / M_PI * 180 + 270);

		if(reversed) {
			cur_angle += 180;
			cur_speed *= -1;
		}

		while(cur_angle >= 360) cur_angle -= 360;
		while(cur_angle < 0) cur_angle += 360;

		servo->update(cur_angle), wheel->update(cur_speed);
		vx_ = vx, vy_ = vy, angle_ = angle;
	}
};

SW sw_1 = { &s_1, &w_1 }, sw_2 = { &s_2, &w_2 }, sw_3 = { &s_3, &w_3 }, sw_4 = { &s_4, &w_4 };

float target = 0;
bool dir = false;

void set_target(bsp_uart_e e, uint8_t *s, uint16_t l) {
	sscanf((char *) s, "%f", &target);
}

void motor_update(double vx, double vy, double rotate) {
	// Servo 1, rotate v = ( 1,  1)
	auto x1 =  rotate / M_SQRT2 + vx, y1 =  rotate / M_SQRT2 + vy;
	// Servo 2, rotate v = ( 1, -1)
	auto x2 =  rotate / M_SQRT2 + vx, y2 = -rotate / M_SQRT2 + vy;
	// Servo 3, rotate v = (-1, -1)
	auto x3 = -rotate / M_SQRT2 + vx, y3 = -rotate / M_SQRT2 + vy;
	// Servo 4, rotate v = (-1,  1)
	auto x4 = -rotate / M_SQRT2 + vx, y4 =  rotate / M_SQRT2 + vy;

	sw_1.update(x1, y1); sw_2.update(x2, y2); sw_3.update(x3, y3); sw_4.update(x4, y4);
}

const auto rc = bsp_rc_data();

// 静态任务，在 CubeMX 中配置
void app_chassis_task(void *args) {
	// Wait for system init.
	while(!app_sys_ready()) OS::Task::SleepMilliseconds(10);

	int zero_count = 0;
	bsp_uart_set_callback(E_UART_DEBUG, set_target);

	double vx = 0, vy = 0, rotate = 0;
	const double eps = 1e-7;

	while(true) {
		vx = rc->rc_l[0] * 2.5, vy = rc->rc_l[1] * 2.5, rotate = rc->reserved * 2.0;

		if(rotate == 0) rotate = eps;

		if(vx == 0 and vy == 0) {
			if(rotate > eps or zero_count == 500) {
				sw_1.update( rotate / M_SQRT2,  rotate / M_SQRT2);
				sw_2.update( rotate / M_SQRT2, -rotate / M_SQRT2);
				sw_3.update(-rotate / M_SQRT2, -rotate / M_SQRT2);
				sw_4.update(-rotate / M_SQRT2,  rotate / M_SQRT2);
			} else {
				zero_count ++;
				motor_update(vx, vy, rotate);
			}
		} else {
			zero_count = 0;
			motor_update(vx, vy, rotate);
		}

		OS::Task::SleepMilliseconds(1);
	}
}

void app_chassis_init() {
	w_1.init(); w_2.init(); w_3.init(); w_4.init();
	w_1.add_controller(std::make_unique <MotorBasePID> (
		MotorBasePID::PID_SPEED,
		std::make_unique <PID> (15, 0.5, 0.0, 16384, 1000),
		nullptr
	));
	w_2.add_controller(std::make_unique <MotorBasePID> (
		MotorBasePID::PID_SPEED,
		std::make_unique <PID> (15, 0.5, 0.0, 16384, 1000),
		nullptr
	));
	w_3.add_controller(std::make_unique <MotorBasePID> (
		MotorBasePID::PID_SPEED,
		std::make_unique <PID> (15, 0.5, 0.0, 16384, 1000),
		nullptr
	));
	w_4.add_controller(std::make_unique <MotorBasePID> (
		MotorBasePID::PID_SPEED,
		std::make_unique <PID> (15, 0.5, 0.0, 16384, 1000),
		nullptr
	));
	s_1.init(); s_2.init(); s_3.init(); s_4.init();
	s_1.add_controller(std::make_unique <MotorBasePID> (
		MotorBasePID::PID_SPEED | MotorBasePID::PID_ANGLE,
		std::make_unique <PID> (750, 8.5, 0.0, 16384, 7500),
		std::make_unique <PID> (2, 0, 0, 150, 0),
		true
	));
	s_1.add_controller(std::make_unique <LowPassFilter> (100, 0.001));
	s_2.add_controller(std::make_unique <LowPassFilter> (100, 0.001));
	s_2.add_controller(std::make_unique <MotorBasePID> (
		MotorBasePID::PID_SPEED | MotorBasePID::PID_ANGLE,
		std::make_unique <PID> (750, 8.5, 0.0, 16384, 7500),
		std::make_unique <PID> (2, 0, 0, 150, 0),
		true
	));
	s_2.add_controller(std::make_unique <LowPassFilter> (100, 0.001));
	s_3.add_controller(std::make_unique <LowPassFilter> (100, 0.001));
	s_3.add_controller(std::make_unique <MotorBasePID> (
		MotorBasePID::PID_SPEED | MotorBasePID::PID_ANGLE,
		std::make_unique <PID> (750, 8.5, 0.0, 16384, 7500),
		std::make_unique <PID> (2, 0, 0, 150, 0),
		true
	));
	s_3.add_controller(std::make_unique <LowPassFilter> (100, 0.001));
	s_4.add_controller(std::make_unique <LowPassFilter> (100, 0.001));
	s_4.add_controller(std::make_unique <MotorBasePID> (
		MotorBasePID::PID_SPEED | MotorBasePID::PID_ANGLE,
		std::make_unique <PID> (750, 8.5, 0.0, 16384, 7500),
		std::make_unique <PID> (2, 0, 0, 150, 0),
		true
	));
	s_4.add_controller(std::make_unique <LowPassFilter> (100, 0.001));
	s_1.use_degree_angle = true; s_1.encoder_zero = 1114;
	s_2.use_degree_angle = true; s_2.encoder_zero = 1720;
	s_3.use_degree_angle = true; s_3.encoder_zero = 1108;
	s_4.use_degree_angle = true; s_4.encoder_zero =  478;
}

#endif