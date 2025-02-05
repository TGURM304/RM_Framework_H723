//
// Created by fish on 2025/1/22.
//

#include "bsp_flash.h"

#ifdef USE_EXTERNAL_FLASH
#include "w25q64.h"
#else
#include "ee.h"
#endif

bsp_flash_data_t flash;

void bsp_flash_init() {
#ifdef USE_EXTERNAL_FLASH
    OSPI_W25Qxx_Init();
#else
    EE_Init(&flash, sizeof flash);
#endif
}

void bsp_flash_read() {
#ifdef USE_EXTERNAL_FLASH
    OSPI_W25Qxx_ReadBuffer((uint8_t *) &flash, 0, sizeof(flash));
#else
    EE_Read();
#endif
}

uint8_t bsp_flash_write() {
#ifdef USE_EXTERNAL_FLASH
    uint8_t err = OSPI_W25Qxx_OK;
    err |= OSPI_W25Qxx_SectorErase(0);
    err |= OSPI_W25Qxx_WriteBuffer((uint8_t *) &flash, 0, sizeof(flash));
    return err == OSPI_W25Qxx_OK;
#else
    return EE_Write();
#endif
}

bsp_flash_data_t *bsp_flash_data() {
    return &flash;
}