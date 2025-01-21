//
// Created by fish on 2025/1/22.
//

#ifndef BSP_FLASH_H
#define BSP_FLASH_H

#include <stdint.h>

typedef struct {
    uint32_t sys_flag;
    double imu_cali[3];
} bsp_flash_data_t;

#ifdef __cplusplus
extern "C" {
#endif

void bsp_flash_init();
void bsp_flash_read();
void bsp_flash_write();
bsp_flash_data_t *bsp_flash_data();

#ifdef __cplusplus
}
#endif
#endif //BSP_FLASH_H
