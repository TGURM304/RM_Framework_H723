//
// Created by fish on 2025/3/1.
//

#pragma once

#include "bsp_rc.h"
#include <cstdint>

/* Referee Data */
//这里主要存放了由裁判系统读取到的数据

/**
 * @brief 裁判系统 - 比赛状态数据 (0x0001)
 */
struct app_referee_game_status_t {
    uint8_t game_type : 4;          ///< 比赛类型
    uint8_t game_progress : 4;      ///< 当前比赛阶段
    uint16_t stage_remain_time;     ///< 当前阶段剩余时间 (s)
    uint64_t SyncTimeStamp;         ///< 同步时间戳
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 比赛结果数据 (0x0002)
 */
struct app_referee_game_result_t {
    uint8_t winner; ///< 获胜方 (1: 红方, 2: 蓝方, 0: 平局)
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 机器人血量数据 (0x0003)
 */
struct app_referee_game_robot_hp_t {
    uint16_t red_1_robot_HP;    ///< 红方1号机器人血量
    uint16_t red_2_robot_HP;    ///< 红方2号机器人血量
    uint16_t red_3_robot_HP;    ///< 红方3号机器人血量
    uint16_t red_4_robot_HP;    ///< 红方4号机器人血量
    uint16_t red_reserved;      ///< 红方保留
    uint16_t red_7_robot_HP;    ///< 红方前哨站
    uint16_t red_outpost_HP;    ///< 红方前哨站血量
    uint16_t red_base_HP;       ///< 红方基地血量
    uint16_t blue_1_robot_HP;   ///< 蓝方1号机器人血量
    uint16_t blue_2_robot_HP;   ///< 蓝方2号机器人血量
    uint16_t blue_3_robot_HP;   ///< 蓝方3号机器人血量
    uint16_t blue_4_robot_HP;   ///< 蓝方4号机器人血量
    uint16_t blue_reserved;     ///< 蓝方保留
    uint16_t blue_7_robot_HP;   ///< 蓝方7号机器人血量
    uint16_t blue_outpost_HP;   ///< 蓝方前哨站血量
    uint16_t blue_base_HP;      ///< 蓝方基地血量
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 机器人状态数据 (0x0201)
 */
struct app_referee_robot_status_t {
    uint8_t robot_id;                       ///< 机器人ID
    uint8_t robot_level;                    ///< 机器人等级
    uint16_t current_HP;                    ///< 当前血量
    uint16_t maximum_HP;                    ///< 最大血量
    uint16_t shooter_barrel_cooling_value;  ///< 枪口冷却值
    uint16_t shooter_barrel_heat_limit;     ///< 枪口热量上限
    uint16_t chassis_power_limit;           ///< 底盘功率上限
    uint8_t power_management_gimbal_output : 1;  ///< 云台能量输出状态
    uint8_t power_management_chassis_output : 1; ///< 底盘能量输出状态
    uint8_t power_management_shooter_output : 1; ///< 射击能量输出状态
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 功率与热量数据 (0x0202)
 */
struct app_referee_power_heat_data_t {
    uint64_t reserved;                  ///< 保留字段
    uint16_t buffer_energy;             ///< 缓冲能量
    uint16_t shooter_17mm_1_barrel_heat;///< 17mm枪口1热量
    uint16_t shooter_17mm_2_barrel_heat;///< 17mm枪口2热量
    uint16_t shooter_42mm_barrel_heat;  ///< 42mm枪口热量
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 机器人位置信息 (0x0203)
 */
struct app_referee_robot_pos_t {
    float x;      ///< x坐标
    float y;      ///< y坐标
    float angle;  ///< 朝向角度
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 机器人增益信息 (0x0204)
 */
struct app_referee_robot_buff_t {
    uint8_t recovery_buff;       ///< 回复增益
    uint8_t cooling_buff;        ///< 冷却增益
    uint8_t defence_buff;        ///< 防御增益
    uint8_t vulnerability_buff;  ///< 易伤增益
    uint16_t attack_buff;        ///< 攻击增益
    uint8_t remaining_energy;    ///< 剩余能量
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 伤害数据 (0x0206)
 */
struct app_referee_hurt_data_t {
    uint8_t armor_id : 4;            ///< 装甲板ID
    uint8_t HP_deduction_reason : 4; ///< 扣血原因
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 射击数据 (0x0207)
 */
struct app_referee_shoot_data_t {
    uint8_t bullet_type;         ///< 子弹类型
    uint8_t shooter_number;      ///< 射击口编号
    uint8_t launching_frequency; ///< 发射频率
    float initial_speed;         ///< 初始速度
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 弹丸剩余量 (0x0208)
 */
struct app_referee_projectile_allowance_t {
    uint16_t projectile_allowance_17mm; ///< 17mm弹丸剩余量
    uint16_t projectile_allowance_42mm; ///< 42mm弹丸剩余量
    uint16_t remaining_gold_coin;       ///< 剩余金币数
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - RFID状态 (0x0209)
 */
struct app_referee_rfid_status_t {
    uint32_t rfid_status; ///< RFID状态位
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 地面机器人位置信息 (0x020B)
 */
struct app_referee_ground_robot_position_t {
    float hero_x, hero_y;           ///< 英雄机器人坐标
    float engineer_x, engineer_y;   ///< 工程机器人坐标
    float standard_3_x, standard_3_y; ///< 3号机器人坐标
    float standard_4_x, standard_4_y; ///< 4号机器人坐标
    uint64_t reserved;              ///< 保留字段
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 哨兵信息 (0x020D)
 */
struct app_referee_sentry_info_t {
    uint32_t sentry_info;   ///< 哨兵信息字段1
    uint16_t sentry_info_2; ///< 哨兵信息字段2
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 自定义控制器（图传链路，0x0302）
 */
struct app_referee_custom_controller_t {
    float joint[6]; ///< 关节角度数组
    struct {
        bool key1 : 1; bool key2 : 1; bool key3 : 1; bool key4 : 1;
        bool key5 : 1; bool key6 : 1; bool key7 : 1; bool key8 : 1;
        bool key9 : 1; bool key10 : 1; bool key11 : 1; bool key12 : 1;
        bool key13 : 1; bool key14 : 1; bool key15 : 1; bool key16 : 1;
    } __attribute__ ((packed)) key; ///< 控制器按键状态
    float reserved; ///< 保留字段
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 客户端控制（图传链路，0x0304）
 */
struct app_referee_remote_control_t {
    int16_t mouse_x, mouse_y, mouse_z; ///< 鼠标位移
    uint8_t mouse_l, mouse_r;          ///< 鼠标左右键
    bsp_rc_keyboard_u keyboard;        ///< 键盘状态
    uint16_t reserved;                 ///< 保留字段
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 裁判系统数据总表
 */
struct app_referee_data_t {
    app_referee_game_status_t game_status;           ///< 比赛状态
    app_referee_game_result_t game_result;           ///< 比赛结果
    app_referee_game_robot_hp_t game_robot_hp;       ///< 血量信息
    app_referee_robot_status_t robot_status;         ///< 机器人状态
    app_referee_power_heat_data_t power_heat_data;   ///< 功率热量信息
    app_referee_robot_pos_t robot_pos;               ///< 机器人位置
    app_referee_robot_buff_t robot_buff;             ///< 增益信息
    app_referee_hurt_data_t hurt_data;               ///< 伤害数据
    app_referee_shoot_data_t shoot_data;             ///< 射击数据
    app_referee_projectile_allowance_t projectile_allowance; ///< 弹丸剩余量
    app_referee_rfid_status_t rfid_status;           ///< RFID状态
    app_referee_ground_robot_position_t ground_robot_position; ///< 地面机器人位置
    app_referee_sentry_info_t sentry_info;           ///< 哨兵信息
    app_referee_custom_controller_t custom_controller; ///< 自定义控制器
    app_referee_remote_control_t remote_control;     ///< 客户端控制
    unsigned int timestamp;                          ///< 数据时间戳
    unsigned int custom_controller_timestamp;        ///< 自定义控制器时间戳
};


/* UI */

/**
 * @brief 裁判系统 - 客户端图形绘制命令 (0x0301)
 */
struct app_referee_client_draw_t {
    uint8_t operate_type;     ///< 操作类型 (1=新增, 2=修改, 3=删除)
    uint8_t layer;            ///< 图层编号
    char name[3];             ///< 图形名称 (ASCII字符)
    uint8_t color;            ///< 颜色 (0=红, 1=蓝, 2=黄, 3=绿, 4=青, 5=粉, 6=白)
    uint8_t figure_type;      ///< 图形类型 (线, 矩形, 圆等)
    uint16_t start_x;         ///< 起点x坐标
    uint16_t start_y;         ///< 起点y坐标
    uint16_t end_x;           ///< 终点x坐标
    uint16_t end_y;           ///< 终点y坐标
    uint16_t radius;          ///< 半径 (仅对圆/圆弧有效)
    uint8_t line_width;       ///< 线宽
    uint8_t start_angle;      ///< 起始角度 (圆弧专用)
    uint8_t end_angle;        ///< 终止角度 (圆弧专用)
    uint8_t reserved;         ///< 保留字段
} __attribute__ ((packed));

/**
 * @brief 裁判系统 - 客户端字符串绘制命令 (0x0303)
 */
struct app_referee_client_string_t {
    uint8_t operate_type;     ///< 操作类型 (1=新增, 2=修改, 3=删除)
    uint8_t layer;            ///< 图层编号
    char name[3];             ///< 字符串名称 (ASCII字符)
    uint8_t color;            ///< 字体颜色
    uint16_t start_x;         ///< 起点x坐标
    uint16_t start_y;         ///< 起点y坐标
    uint8_t font_size;        ///< 字号大小
    uint8_t length;           ///< 字符串长度
    char string[30];          ///< 实际字符串内容 (最多30字节)
} __attribute__ ((packed));
