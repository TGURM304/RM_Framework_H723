//
// Created by fish on 2024/10/30.
//

#ifndef BSP_RC_H
#define BSP_RC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

/*
 *  用来存放经过简单处理的遥控器数据。
 *  rc_l/r[2]: 摇杆悬空状态为 {0, 0}，以摇杆中心建立平面直角坐标系，范围 [-660, 660]（向上、向右为正方向）
 *  s_l/r: 左右拨杆悬空状态为 0，向上为 1，向下为 -1
 *  mouse_x/y/z: 鼠标坐标，范围 [-32768, 32767]
 *  mouse_l/r: 鼠标左/右键，范围 [0, 1]
 *  keyboard: 键盘，其中 8 个 bit 分别表示 W、S、A、D、Q、E、Shift、Ctrl
 *  reserved: 官方文档中的保留字段，是遥控器左上角的拨轮，范围 [-660, 660]（向右为正方向）
 */
typedef struct {
    int16_t rc_l[2], rc_r[2];
    int8_t s_l, s_r;
    int16_t mouse_x, mouse_y, mouse_z;
    uint8_t mouse_l, mouse_r;
    uint16_t keyboard;
    int16_t reserved;
} bsp_rc_data_t;

const bsp_rc_data_t *bsp_rc_data();
void bsp_rc_init();

#ifdef __cplusplus
}
#endif

#endif //BSP_RC_H
