//
// Created by fish on 2025/1/22.
//

#include "bsp_flash.h"

#include "ee.h"

bsp_flash_data_t flash;

void bsp_flash_init() {
    EE_Init(&flash, sizeof flash);
}

void bsp_flash_read() {
    EE_Read();
}

void bsp_flash_write() {
    EE_Write();
}

bsp_flash_data_t *bsp_flash_data() {
    return &flash;
}