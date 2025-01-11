//
// Created by fish on 2025/1/1.
//

#include "app_ins.h"

#include "alg_pid.h"
#include "alg_quaternion_ekf.h"
#include "bsp_adc.h"
#include "bsp_def.h"
#include "bsp_uart.h"
#include "sys_task.h"
#include "tim.h"

#define TEMPERATURE_COUNT 100
#define GYRO_CORRECT_SAMPLE_COUNT 1500

#define IMU_TEMPERATURE_CONTROL_TIMER &htim3
#define IMU_TEMPERATURE_CONTROL_CHANNEL TIM_CHANNEL_4

static bool inited_ = false;
static app_ins_data_t data;

Algorithm::PID temp_pid(Algorithm::PID::NONE);

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
	inited_ = true;
}

uint8_t ins_flag = 0;
float gyro_correct[3];

void app_ins_task(void *args) {
	while(!inited_) OS::Task::SleepMilliseconds(10);

	int count = 0, freq_cnt = 0;

	while(true) {
		bsp_imu_read(&data.raw);

		if(++freq_cnt == 100)
			__HAL_TIM_SetCompare(
				IMU_TEMPERATURE_CONTROL_TIMER,
				IMU_TEMPERATURE_CONTROL_CHANNEL,
				std::max(0.0, temp_pid.update(data.raw.temp, 40))
			), freq_cnt = 0;

		if(ins_flag == 0) {
			count += (std::abs(data.raw.temp - 40) < 1.0f);
			if(count == TEMPERATURE_COUNT) ins_flag ++, count = 0;
		}
		if(ins_flag == 1) {
			if(++count <= GYRO_CORRECT_SAMPLE_COUNT) {
				gyro_correct[0] += data.raw.gyro[0];
				gyro_correct[1] += data.raw.gyro[1];
				gyro_correct[2] += data.raw.gyro[2];
			}
			if(count == GYRO_CORRECT_SAMPLE_COUNT) {
				gyro_correct[0] /= GYRO_CORRECT_SAMPLE_COUNT;
				gyro_correct[1] /= GYRO_CORRECT_SAMPLE_COUNT;
				gyro_correct[2] /= GYRO_CORRECT_SAMPLE_COUNT;
				ins_flag ++, count = 0;
			}
		}
		if(ins_flag == 2) {
			float gyro[3] = {
				data.raw.gyro[0] - gyro_correct[0],
				data.raw.gyro[1] - gyro_correct[1],
				data.raw.gyro[2] - gyro_correct[2]
			};
			IMU_QuaternionEKF_Update(
				gyro[0], gyro[1], gyro[2],
				data.raw.accel[0], data.raw.accel[1], data.raw.accel[2]
			);
			std::tie(data.roll, data.pitch, data.yaw) = IMU_QuaternionEKF_Data();
		}

		OS::Task::SleepMilliseconds(1);
	}
}

uint8_t app_ins_status() {
	return ins_flag;
}

const app_ins_data_t *app_ins_data() {
	return &data;
}