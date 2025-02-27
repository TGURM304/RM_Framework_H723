//
// Created by fish on 2025/2/6.
//

#include "app_sys_err.h"
#include <bitset>

std::bitset<SYS_ERR_SIZE> errors;

bool app_sys_err() {
    return errors.any();
}

void app_sys_err_mark(app_sys_err_t err) {
    errors[err] = true;
}

bool app_sys_err_check(app_sys_err_t err) {
    return errors[err];
}

void app_sys_err_clear() {
    errors.reset();
}
