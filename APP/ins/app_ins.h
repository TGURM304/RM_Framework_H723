//
// Created by fish on 2025/1/1.
//

#ifndef APP_INS_H
#define APP_INS_H
#include <cstdint>

#include "bsp_imu.h"

struct app_ins_data_t {
	float yaw, pitch, roll;
	bsp_imu_raw_data_t raw;
};

#ifdef __cplusplus
extern "C" {
#endif

void app_ins_init();
void app_ins_task(void *args);
uint8_t app_ins_status();
const app_ins_data_t *app_ins_data();

#ifdef __cplusplus
}
#endif

#endif //APP_INS_H
