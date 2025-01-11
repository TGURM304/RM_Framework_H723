//
// Created by fish on 2024/12/18.
//

#pragma once

#include <cstdint>

namespace vision {
    // rm-vision 通信协议，参考：https://flowus.cn/lihanchen/share/6546079f-b4e6-4156-9553-8cf4a5e77bb9
    struct SendPacket {
        uint8_t header = 0x5A;
        uint8_t detect_color : 1;  // 0-red 1-blue
        bool reset_tracker : 1;
        uint8_t reserved : 6;
        float roll;
        float pitch;
        float yaw;
        float aim_x;
        float aim_y;
        float aim_z;
        uint16_t checksum = 0;
    } __attribute__((packed));

    // 对应的弹道解算模型和 x, y, z 含义参考：https://github.com/CodeAlanqian/SolveTrajectory
    struct RecvPacket {
        uint8_t header = 0xA5;
        bool tracking : 1;          // 当前是否锁定目标
        uint8_t id : 3;             // 0-outpost 6-guard 7-base
        uint8_t armors_num : 3;     // 2-balance 3-outpost 4-normal
        uint8_t reserved : 1;
        float x;                    // 目标在世界坐标系下的 x 坐标
        float y;                    // 目标在世界坐标系下的 y 坐标
        float z;                    // 目标在世界坐标系下的 z 坐标
        float yaw;                  // 目标在世界坐标系下的倾斜角度
        float vx;                   // 目标在世界坐标系下 x 方向的速度
        float vy;                   // 目标在世界坐标系下 y 方向的速度
        float vz;                   // 目标在世界坐标系下 z 方向的速度
        float v_yaw;                // 目标旋转的角速度
        float r1;                   // 目标其中一组装甲板相对中心的半径
        float r2;                   // 目标另一组装甲板相对中心的半径
        float dz;                   // tracking 中的装甲板的上一块装甲板的 z 轴位置
        uint16_t checksum = 0;
    } __attribute__((packed));

    void init();
    RecvPacket *recv();
    void send(uint8_t detect_color, bool reset_tracker);
}