//
// Created by fish on 2024/9/18.
//

#include "bsp_imu.h"
#include "BMI088driver.h"

void bsp_imu_read(bsp_imu_raw_data_t *data) {
    BMI088_read(data->gyro, data->accel, &data->temp);
}

void bsp_imu_init() {
    while(BMI088_init());
}
