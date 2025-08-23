//
// Created by fish on 2024/9/18.
//

#include "bsp_imu.h"
#include "BMI088driver.h"
/**
 * @brief  读取陀螺仪数据
 *
 * @param  data 陀螺仪原始数据
 */
void bsp_imu_read(bsp_imu_raw_data_t *data) {
    BMI088_read(data->gyro, data->accel, &data->temp);
}

/**
 * @brief  陀螺仪初始化函数
 *
 */
void bsp_imu_init() {
    while(BMI088_init());
}
