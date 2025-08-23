//
// Created by fish on 2025/2/6.
//

#include "app_sys_err.h"
#include <bitset>

/* 系统错误位集合，每一位对应一个错误 */
std::bitset<SYS_ERR_SIZE> errors;

/**
 * @brief 检查系统是否存在错误
 *
 * @return true  存在至少一个错误
 * @return false 没有错误
 */
bool app_sys_err() {
    return errors.any();
}

/**
 * @brief 标记一个系统错误
 *
 * @param err 错误类型
 */
void app_sys_err_mark(app_sys_err_t err) {
    errors[err] = true;
}

/**
 * @brief 检查指定错误是否存在
 *
 * @param err 错误类型
 * @return true  指定错误存在
 * @return false 指定错误不存在
 */
bool app_sys_err_check(app_sys_err_t err) {
    return errors[err];
}

/**
 * @brief 清除所有系统错误
 */
void app_sys_err_clear() {
    errors.reset();
}
