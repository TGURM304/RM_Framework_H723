//
// Created by fish on 2025/3/1.
//

#pragma once

#include "bsp_rc.h"
#include <cstdint>

/* Referee Data */

// 0x0001
struct app_referee_game_status_t {
    uint8_t game_type : 4;
    uint8_t game_progress : 4;
    uint16_t stage_remain_time;
    uint64_t SyncTimeStamp;
} __attribute__ ((packed));

// 0x0002
struct app_referee_game_result_t {
    uint8_t winner;
} __attribute__ ((packed));

// 0x0003
struct app_referee_game_robot_hp_t {
    uint16_t red_1_robot_HP;
    uint16_t red_2_robot_HP;
    uint16_t red_3_robot_HP;
    uint16_t red_4_robot_HP;
    uint16_t red_reserved;
    uint16_t red_7_robot_HP;
    uint16_t red_outpost_HP;
    uint16_t red_base_HP;
    uint16_t blue_1_robot_HP;
    uint16_t blue_2_robot_HP;
    uint16_t blue_3_robot_HP;
    uint16_t blue_4_robot_HP;
    uint16_t blue_reserved;
    uint16_t blue_7_robot_HP;
    uint16_t blue_outpost_HP;
    uint16_t blue_base_HP;
} __attribute__ ((packed));

// 0x0201
struct app_referee_robot_status_t {
    uint8_t robot_id;
    uint8_t robot_level;
    uint16_t current_HP;
    uint16_t maximum_HP;
    uint16_t shooter_barrel_cooling_value;
    uint16_t shooter_barrel_heat_limit;
    uint16_t chassis_power_limit;
    uint8_t power_management_gimbal_output : 1;
    uint8_t power_management_chassis_output : 1;
    uint8_t power_management_shooter_output : 1;
} __attribute__ ((packed));

// 0x0202
struct app_referee_power_heat_data_t {
    uint64_t reserved;
    uint16_t buffer_energy;
    uint16_t shooter_17mm_1_barrel_heat;
    uint16_t shooter_17mm_2_barrel_heat;
    uint16_t shooter_42mm_barrel_heat;
} __attribute__ ((packed));

// 0x0203
struct app_referee_robot_pos_t {
    float x;
    float y;
    float angle;
} __attribute__ ((packed));

// 0x0204
struct app_referee_robot_buff_t {
    uint8_t recovery_buff;
    uint8_t cooling_buff;
    uint8_t defence_buff;
    uint8_t vulnerability_buff;
    uint16_t attack_buff;
    uint8_t remaining_energy;
} __attribute__ ((packed));

// 0x0206
struct app_referee_hurt_data_t {
    uint8_t armor_id : 4;
    uint8_t HP_deduction_reason : 4;
} __attribute__ ((packed));

// 0x0207
struct app_referee_shoot_data_t {
    uint8_t bullet_type;
    uint8_t shooter_number;
    uint8_t launching_frequency;
    float initial_speed;
} __attribute__ ((packed));

// 0x0208
struct app_referee_projectile_allowance_t {
    uint16_t projectile_allowance_17mm;
    uint16_t projectile_allowance_42mm;
    uint16_t remaining_gold_coin;
} __attribute__ ((packed));

// 0x0209
struct app_referee_rfid_status_t {
    uint32_t rfid_status;
} __attribute__ ((packed));

// 0x020B
struct app_referee_ground_robot_position_t {
    float hero_x;
    float hero_y;
    float engineer_x;
    float engineer_y;
    float standard_3_x;
    float standard_3_y;
    float standard_4_x;
    float standard_4_y;
    uint64_t reserved;
} __attribute__ ((packed));

// 0x020D
struct app_referee_sentry_info_t {
    uint32_t sentry_info;
    uint16_t sentry_info_2;
} __attribute__ ((packed));

// 0x0302 - 自定义控制器（图传链路）
struct app_referee_custom_controller_t {
    float joint[6];
    // uint16_t key;
    struct {
        bool key1 : 1;
        bool key2 : 1;

    } __attribute__ ((packed)) key;
    float reserved;
} __attribute__ ((packed));

// 0x0304 - 客户端控制（图传链路）
struct app_referee_remote_control_t {
    int16_t mouse_x, mouse_y, mouse_z;
    uint8_t mouse_l, mouse_r;
    bsp_rc_keyboard_u keyboard;
    uint16_t reserved;
} __attribute__ ((packed));

struct app_referee_data_t {
    app_referee_game_status_t game_status;
    app_referee_game_result_t game_result;
    app_referee_game_robot_hp_t game_robot_hp;
    app_referee_robot_status_t robot_status;
    app_referee_power_heat_data_t power_heat_data;
    app_referee_robot_pos_t robot_pos;
    app_referee_robot_buff_t robot_buff;
    app_referee_hurt_data_t hurt_data;
    app_referee_shoot_data_t shoot_data;
    app_referee_projectile_allowance_t projectile_allowance;
    app_referee_rfid_status_t rfid_status;
    app_referee_ground_robot_position_t ground_robot_position;
    app_referee_sentry_info_t sentry_info;
    app_referee_custom_controller_t custom_controller;
    app_referee_remote_control_t remote_control;
    unsigned int timestamp, custom_controller_timestamp;
};

/* UI */

enum app_referee_ui_figure_type_e {
    UI_FIGURE_LINE,            // 直线
    UI_FIGURE_RECTANGLE,       // 矩形
    UI_FIGURE_CIRCLE,          // 正圆
    UI_FIGURE_OVAL,            // 椭圆
    UI_FIGURE_ARC,             // 圆弧
    UI_FIGURE_FLOAT,           // 浮点数
    UI_FIGURE_INT,             // 整型数
    UI_FIGURE_STRING           // 字符
};

enum app_referee_ui_color_e {
    UI_COLOR_SELF,
    UI_COLOR_YELLOW,
    UI_COLOR_GREEN,
    UI_COLOR_ORANGE,
    UI_COLOR_PURPLE,
    UI_COLOR_PINK,
    UI_COLOR_CYAN,
    UI_COLOR_BLACK,
    UI_COLOR_WHITE,
};

struct app_referee_robot_interaction_header_t {
    uint16_t data_cmd_id; // 注意这里是子内容 id
    uint16_t sender_id;
    uint16_t receiver_id;
} __attribute__ ((packed));

// 0x0101 - 0x0104
struct app_referee_ui_figure_t {
    char figure_name[3];
    uint32_t operate_type : 3;
    uint32_t figure_type : 3;
    uint32_t layer : 4;
    uint32_t color : 4;
    uint32_t details_a : 9;
    uint32_t details_b : 9;
    uint32_t width : 10;
    uint32_t start_x : 11;
    uint32_t start_y :  11;
    uint32_t details_c : 10;
    uint32_t details_d : 11;
    uint32_t details_e : 11;
} __attribute__ ((packed));

// 0x0110
struct app_referee_ui_string_t {
    char figure_name[3];
    uint32_t operate_type : 3;
    uint32_t figure_type : 3;
    uint32_t layer : 4;
    uint32_t color : 4;
    uint32_t details_a : 9;
    uint32_t details_b : 9;
    uint32_t width : 10;
    uint32_t start_x : 11;
    uint32_t start_y :  11;
    uint32_t details_c : 10;
    uint32_t details_d : 11;
    uint32_t details_e : 11;
    char data[30];
} __attribute__ ((packed));