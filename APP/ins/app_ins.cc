//
// Created by fish on 2025/1/1.
//

#include "app_ins.h"

#include "ctrl_pid.h"
#include "alg_quaternion_ekf.h"
#include "app_terminal.h"
#include "bsp_adc.h"
#include "bsp_buzzer.h"
#include "bsp_def.h"
#include "bsp_uart.h"
#include "bsp_flash.h"
#include "sys_task.h"
#include "tim.h"

#define FLASH_KEYWORD 998244353
#define GYRO_CORRECT_SAMPLE_COUNT 10000

#define IMU_TEMPERATURE_CONTROL_TIMER &htim3
#define IMU_TEMPERATURE_CONTROL_CHANNEL TIM_CHANNEL_4

static bool inited_ = false;
static app_ins_data_t data;

Controller::PID temp_pid;

bool calibrated = false;
double gyro_correct[3];

void app_ins_init() {
	bsp_imu_init();
	BSP_ASSERT(bsp_adc_vbus() > 0);
	if(bsp_adc_vbus() > 22) {
		temp_pid.set_para(100, 1, 0, 10000, 500);
	} else if(bsp_adc_vbus() > 10) {
		temp_pid.set_para(500, 2, 1, 10000, 1000);
	} else {
		temp_pid.set_para(12500, 10, 1, 25000, 10000);
	}
	HAL_TIM_PWM_Start(IMU_TEMPERATURE_CONTROL_TIMER, IMU_TEMPERATURE_CONTROL_CHANNEL);
	IMU_QuaternionEKF_Init(10, 0.001, 10000000, 1, 0.001f, 0);

	auto flash = bsp_flash_data(); bsp_flash_read();
	if(flash->sys_flag == FLASH_KEYWORD) {
		// 从 Flash 中读陀螺仪参数
		memcpy(gyro_correct, flash->imu_cali, 3 * sizeof(double));
		calibrated = true;
	}

	app_terminal_register_cmd("ins", "ins commands", [flash](const auto &args) -> bool {
		auto running = app_terminal_running_flag();
		if(args.size() == 1) {
			TERMINAL_INFO("usage: ins cali/watch/test/config\r\n");
			return true;
		}
		if(args[1] == "cali") {
			calibrated = false;
			TERMINAL_INFO("正在等待陀螺仪温控...\r\n");
			while(std::abs(data.raw.temp - 40.0f) > 0.5) {
				TERMINAL_SEND(TERMINAL_CLEAR_LINE, sizeof TERMINAL_CLEAR_LINE);
				TERMINAL_INFO_PRINTF("当前温度: %f", data.raw.temp);
				OS::Task::SleepMilliseconds(250);
			}
			TERMINAL_SEND(TERMINAL_CLEAR_LINE, sizeof TERMINAL_CLEAR_LINE);
			TERMINAL_INFO_PRINTF("当前温度: %f", data.raw.temp);
			TERMINAL_INFO("\r\n正在校准陀螺仪，校准过程中请勿移动陀螺仪...\r\n");
			int count = GYRO_CORRECT_SAMPLE_COUNT;
			gyro_correct[0] = gyro_correct[1] = gyro_correct[2] = 0;
			while(count -- and *running) {
				gyro_correct[0] += data.raw.gyro[0];
				gyro_correct[1] += data.raw.gyro[1];
				gyro_correct[2] += data.raw.gyro[2];
				TERMINAL_SEND(TERMINAL_CLEAR_LINE, sizeof TERMINAL_CLEAR_LINE);
				TERMINAL_INFO_PRINTF("[%d] %lf, %lf, %lf",
					GYRO_CORRECT_SAMPLE_COUNT - count, gyro_correct[0], gyro_correct[1], gyro_correct[2]
				);
				OS::Task::SleepMilliseconds(1);
			}
			TERMINAL_INFO("\r\n");
			if(!*running) return false;
			gyro_correct[0] /= GYRO_CORRECT_SAMPLE_COUNT;
			gyro_correct[1] /= GYRO_CORRECT_SAMPLE_COUNT;
			gyro_correct[2] /= GYRO_CORRECT_SAMPLE_COUNT;
			TERMINAL_INFO_PRINTF("正在保存数据: %lf, %lf, %lf\r\n", gyro_correct[0], gyro_correct[1], gyro_correct[2]);
			flash->sys_flag = FLASH_KEYWORD;
			memcpy(flash->imu_cali, gyro_correct, 3 * sizeof(double));
			if(bsp_flash_write()) {
				calibrated = true;
				TERMINAL_INFO("校准完成\r\n");
			} else {
				TERMINAL_ERROR("Flash 写入失败\r\n");
			}

			return true;
		}
		if(args[1] == "watch") {
			while(*running) {
				TERMINAL_INFO_PRINTF("%f,%f,%f,%f\r\n", data.roll, data.pitch, data.yaw, data.raw.temp);
				OS::Task::SleepMilliseconds(1);
			}
			return true;
		}
		if(args[1] == "test") {
			if(!calibrated) {
				TERMINAL_ERROR("陀螺仪未校准\r\n");
				return false;
			}
			TERMINAL_INFO("正在测试陀螺仪，测试过程中请勿移动陀螺仪... (5s * 5)\r\n");
			double sum = 0;
			for(int i = 1; i <= 5; i++) {
				double st = data.yaw;
				OS::Task::SleepSeconds(5);
				double ed = data.yaw;
				TERMINAL_INFO_PRINTF("test #%d = %lf deg/min\r\n", i, (ed - st) * 12);
				sum += (ed - st) * 12;
			}
			TERMINAL_INFO_PRINTF("avg = %lf deg/min\r\n", sum / 5);
			return true;
		}
		if(args[1] == "config") {
			TERMINAL_INFO_PRINTF("%lf, %lf, %lf\r\n", gyro_correct[0], gyro_correct[1], gyro_correct[2]);
			return true;
		}
		return false;
	});

	inited_ = true;
}

void app_ins_task(void *args) {
	while(!inited_) OS::Task::SleepMilliseconds(10);

	int freq_cnt = 0;

	while(true) {
		bsp_imu_read(&data.raw);

		if(++freq_cnt == 100)
			__HAL_TIM_SetCompare(
				IMU_TEMPERATURE_CONTROL_TIMER,
				IMU_TEMPERATURE_CONTROL_CHANNEL,
				std::max(0.0f, temp_pid.update(data.raw.temp, 40))
			), freq_cnt = 0;

		if(calibrated) {
			double gyro[3] = {
				data.raw.gyro[0] - gyro_correct[0],
				data.raw.gyro[1] - gyro_correct[1],
				data.raw.gyro[2] - gyro_correct[2]
			};
			IMU_QuaternionEKF_Update(
				static_cast <float> (gyro[0]), static_cast <float> (gyro[1]), static_cast <float> (gyro[2]),
				data.raw.accel[0], data.raw.accel[1], data.raw.accel[2]
			);
			std::tie(data.roll, data.pitch, data.yaw) = IMU_QuaternionEKF_Data();
		}

		OS::Task::SleepMilliseconds(1);
	}
}

uint8_t app_ins_status() {
	return 2 * calibrated;
}

const app_ins_data_t *app_ins_data() {
	return &data;
}