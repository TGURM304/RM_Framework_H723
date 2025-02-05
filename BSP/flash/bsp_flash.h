//
// Created by fish on 2025/1/22.
//

#ifndef BSP_FLASH_H
#define BSP_FLASH_H

#include <stdint.h>

// 使用板载 w25q64
#define USE_EXTERNAL_FLASH

typedef struct {
    uint32_t sys_flag;
    double imu_cali[3];
} bsp_flash_data_t;

#ifdef __cplusplus
extern "C" {
#endif

void bsp_flash_init();
void bsp_flash_read();
uint8_t bsp_flash_write();
bsp_flash_data_t *bsp_flash_data();

#ifdef __cplusplus
}
#endif
#endif //BSP_FLASH_H
