//
// Created by fish on 2025/1/22.
//

#include "bsp_flash.h"

#include "bsp_def.h"
#include "cmsis_os2.h"
#include "easyflash.h"
#include "w25q64.h"

/**
 * @brief  初始化片上 Flash 存储环境
 *
 * @note   调用 EasyFlash 库的初始化函数，判断是否初始化成功
 *
 * @return 1  初始化成功
 * @return 0  初始化失败
 */
uint8_t bsp_flash_init() {
    return easyflash_init() == EF_NO_ERR;
}

/**
 * @brief  从 Flash 中读取指定键值的数据
 *
 * @param  s     键（字符串），对应存储的变量名
 * @param  buf   目标缓冲区指针，用于存放读取的数据
 * @param  len   期望读取的数据长度（字节数）
 *
 * @note   如果 Flash 中该键对应的数据不足 len 字节，则只会读取实际大小
 */
void bsp_flash_read(const char *s, void *buf, size_t len) {
    ef_get_env_blob(s, buf, len, NULL);
}

/**
 * @brief  向 Flash 中写入数据
 *
 * @param  s     键（字符串），对应存储的变量名
 * @param  buf   数据缓冲区指针，指向要写入的数据
 * @param  len   数据长度（字节数）
 *
 * @return 1  写入成功
 * @return 0  写入失败
 *
 * @note   会覆盖同名键下的旧数据
 */
uint8_t bsp_flash_write(const char *s, void *buf, size_t len) {
    return ef_set_env_blob(s, buf, len) == EF_NO_ERR;
}

/**
 * @brief  打印 Flash 中所有环境变量的状态
 *
 * @note   主要用于调试，会将 EasyFlash 管理的所有键值对输出到日志或终端
 */
void bsp_flash_print_status() {
    ef_print_env();
}
