//
// Created by fish on 2025/2/6.
//

#pragma once

#define SYS_ERR_SIZE 10

typedef enum {
    SYS_ERR_FLASH_INTERNAL,
    SYS_ERR_FLASH_WRONG_BRIEF
} app_sys_err_t;

bool app_sys_err();
void app_sys_err_mark(app_sys_err_t err);
bool app_sys_err_check(app_sys_err_t err);
void app_sys_err_clear();