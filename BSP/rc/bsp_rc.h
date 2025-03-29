//
// Created by fish on 2024/10/30.
//

#ifndef BSP_RC_H
#define BSP_RC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include <stdbool.h>
typedef union {
    uint16_t raw;
    struct __attribute__((packed)) {
        bool w : 1, s : 1, a : 1, d : 1, shift : 1, ctrl : 1, q : 1, e : 1,
        r : 1, f : 1, g : 1, z : 1, x : 1, c : 1, v : 1, b : 1;
    } key;
} bsp_rc_keyboard_u;

/*!
 *  用来存放经过简单处理的遥控器数据。
 *  @param rc_l/r 左、右摇杆状态，悬空状态为 {0, 0}，以摇杆中心建立平面直角坐标系，范围 [-660, 660]（向上、向右为正方向）
 *  @param s_l/r 左、右拨杆状态，悬空状态为 0，向上为 1，向下为 -1
 *  @param mouse_x/y/z 鼠标坐标，范围 [-32768, 32767]
 *  @param mouse_l/r 鼠标左/右键，范围 [0, 1]
 *  @param keyboard 键盘，其中 8 个 bit 分别表示 W、S、A、D、Q、E、Shift、Ctrl
 *  @param reserved 官方文档中的保留字段，是遥控器左上角的拨轮，范围 [-660, 660]（向右为正方向）
 */
typedef struct {
    int16_t rc_l[2], rc_r[2];
    int8_t s_l, s_r;
    int16_t mouse_x, mouse_y, mouse_z;
    uint8_t mouse_l, mouse_r;
    bsp_rc_keyboard_u keyboard;
    int16_t reserved;
    uint32_t timestamp;
} bsp_rc_data_t;

/*!
 * 初始化遥控器
 */
void bsp_rc_init();

/*!
 * 获取遥控器数据指针
 * @return 遥控器数据指针
 */
const bsp_rc_data_t *bsp_rc_data();

#ifdef __cplusplus
}
#endif

#endif //BSP_RC_H