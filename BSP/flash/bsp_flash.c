//
// Created by fish on 2025/1/22.
//

#include "bsp_flash.h"

#include "bsp_def.h"
#include "cmsis_os2.h"
#include "easyflash.h"
#include "w25q64.h"

uint8_t bsp_flash_init() {
    return easyflash_init() == EF_NO_ERR;
}

void bsp_flash_read(const char *s, void *buf, size_t len) {
    ef_get_env_blob(s, buf, len, NULL);
}

uint8_t bsp_flash_write(const char *s, void *buf, size_t len) {
    return ef_set_env_blob(s, buf, len) == EF_NO_ERR;
}

void bsp_flash_print_status() {
    ef_print_env();
}