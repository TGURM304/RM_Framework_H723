//
// Created by fish on 2026/2/3.
//

#pragma once
#include <cstdint>

// RoboMaster 2026 机甲大师高校系列赛通信协议 V1.1.0（20251217）
namespace robomaster {
    // 帧头
    struct frame_header_t {
        uint8_t sof;                        // 数据帧起始字节,固定值为 0xA5
        uint16_t data_length;               // 数据帧中 data 的长度
        uint8_t seq;                        // 包序号
        uint8_t crc;                        // 帧头 CRC8 校验
    } __attribute__((packed));

    namespace basic {
        // 0x0001 比赛状态数据, 固定以 1Hz 频率发送, 服务器->全体机器人
        struct game_status_t {
            uint8_t game_type : 4;          // 比赛类型: 1-超对, 2-单项, 3-人工智能挑战赛, 4-联盟赛3v3, 5-联盟赛1v1
            uint8_t game_progress : 4;      // 比赛阶段: 0-未开始, 1-准备阶段, 2-十五秒自检, 3-五秒倒计时, 4-比赛中, 5-结算中
            uint16_t stage_remain_time;     // 当前阶段剩余时间 (秒)
            uint64_t SyncTimeStamp;         // UNIX 时间, 当机器人正确连接到裁判系统的 NTP 服务器后生效
        } __attribute__((packed));
        // 0x0002 比赛结果数据, 比赛结束触发发送, 服务器->全体机器人
        struct game_result_t {
            uint8_t winner;                 // 0: 平局, 1: 红方获胜, 2: 蓝方获胜
        } __attribute__((packed));
        // 0x0003 机器人血量数据, 固定以 3Hz 频率发送, 服务器->全体机器人
        struct game_robot_hp_t {
            uint16_t ally_1_robot_hp;       // 己方 1 号英雄机器人血量, 若该机器人未上场或者被罚下, 则血量为 0, 下文同理
            uint16_t ally_2_robot_hp;       // 己方 2 号工程机器人血量
            uint16_t ally_3_robot_hp;       // 己方 3 号步兵机器人血量
            uint16_t ally_4_robot_hp;       // 己方 4 号步兵机器人血量
            uint16_t reserved;
            uint16_t ally_7_robot_hp;       // 己方 7 号哨兵机器人血量
            uint16_t ally_outpost_hp;       // 己方前哨站血量
            uint16_t ally_base_hp;          // 己方基地血量
        } __attribute__((packed));
        // 0x0101 场地事件数据, 固定以 1Hz 频率发送, 服务器->己方全体机器人
        struct event_data_t {
            uint32_t event_data;            // 各位数据表示见文档
        } __attribute__((packed));
        // 0x0104 裁判警告数据, 己方判罚/判负时触发发送, 其余时间以 1Hz 频率发送, 服务器->被判罚方全体机器人
        struct referee_warning_t {
            uint8_t level;                  // 己方最后一次受到判罚的等级
            uint8_t offending_robot_id;     // 己方最后一次受到判罚的违规机器人 ID, 红 1 为 1, 蓝 1 为 101
            uint8_t count;                  // 己方最后一次受到判罚的违规机器人对应判罚等级的违规次数
        } __attribute__((packed));
        // 0x0105 飞镖发射相关数据, 固定以 1Hz频率发送, 服务器->己方全体机器人
        struct dart_info_t {
            uint8_t dart_remaining_time;    // 己方飞镖发射剩余时间 (秒)
            uint16_t dart_info;             // 飞镖状态信息, 各位数据表示见文档
        } __attribute__((packed));

        // 0x0201 机器人性能体系数据, 固定以 10Hz 频率发送, 主控模块->对应机器人
        struct robot_status_t {
            uint8_t robot_id;                               // 本机器人 ID
            uint8_t robot_level;                            // 机器人等级
            uint16_t current_hp;                            // 机器人当前血量
            uint16_t maximum_hp;                            // 机器人血量上限
            uint16_t shooter_barrel_cooling_value;          // 机器人射击热量每秒冷却值
            uint16_t shooter_barrel_heat_limit;             // 机器人射击热量上限
            uint16_t chassis_power_limit;                   // 机器人底盘功率上限
            uint8_t power_management_gimbal_output : 1;     // 电源管理模块 gimbal 口输出情况
            uint8_t power_management_chassis_output : 1;    // 电源管理模块 chassis 口输出情况
            uint8_t power_management_shooter_output : 1;    // 电源管理模块 shooter 口输出情况
        } __attribute__((packed));

        // 0x0202 实时底盘缓冲能量和射击热量数据, 固定以 10Hz 频率发送, 主控模块->对应机器人
        struct power_heat_data_t {
            uint16_t reserved_1;
            uint16_t reserved_2;
            float reserved_3;
            uint16_t buffer_energy;                 // 缓冲能量 (J)
            uint16_t shooter_17mm_1_barrel_heat;    // 第 1 个 17mm 发射机构的射击热量
            uint16_t shooter_42mm_barrel_heat;      // 42mm 发射机构的射击热量
        } __attribute__((packed));

        // 0x0203 机器人位置数据, 固定以 1Hz 频率发送, 主控模块->对应机器人
        struct robot_pos_t {
            float x;                                // 本机器人位置 x 坐标, 单位：m
            float y;                                // 本机器人位置 y 坐标, 单位：m
            float angle;                            // 本机器人测速模块的朝向, 单位：度, 正北为 0 度
        } __attribute__((packed));

        // 0x0204 机器人增益和底盘能量数据, 固定以 3Hz 频率发送, 服务器->对应机器人
        struct buff_t {
            uint8_t recovery_buff;                  // 机器人回血增益（百分比, 值为 10 表示每秒恢复血量上限的 10%）
            uint16_t cooling_buff;                  // 机器人射击热量冷却增益具体值（直接值, 值为 x 表示热量冷却增加 x/s）
            uint8_t defence_buff;                   // 机器人防御增益（百分比, 值为 50 表示 50%防御增益）
            uint8_t vulnerability_buff;             // 机器人负防御增益（百分比, 值为 30 表示-30%防御增益）
            uint16_t attack_buff;                   // 机器人攻击增益（百分比, 值为 50 表示 50%攻击增益）
            uint8_t remaining_energy;               // 机器人剩余能量值反馈, 具体表示见文档
        } __attribute__((packed));

        // 0x0206 伤害状态数据, 伤害发生后发送, 主控模块->对应机器人
        struct hurt_data_t {
            uint8_t armor_id;                       // 当扣血原因为装甲模块被弹丸攻击、受撞击或离线时, 该 4 bit 组成的数值为装甲模块或测速模块的 ID 编号；当其他原因导致扣血时, 该数值为 0
            uint8_t hp_deduction_reason;            // 血量变化类型: 0-装甲模块被弹丸攻击导致扣血, 1-装甲模块或超级电容管理模块离线导致扣血, 5-装甲模块受到撞击导致扣血
        } __attribute__((packed));

        // 0x0207 实时射击数据, 弹丸发射后发送, 主控模块->对应机器人
        struct shoot_data_t {
            uint8_t bullet_type;                    // 弹丸类型
            uint8_t shooter_number;                 // 发射机构 ID
            uint8_t launching_frequency;            // 弹丸射速（单位：Hz）
            float initial_speed;                    // 弹丸初速度（单位：m/s）
        } __attribute__((packed));

        // 0x0208 允许发弹量, 固定以 10Hz 频率发送, 服务器->己方英雄、步兵、哨兵、空中机器人
        struct projectile_allowance_t {
            uint16_t projectile_allowance_17mm;     // 机器人自身拥有的 17mm 弹丸允许发弹量
            uint16_t projectile_allowance_42mm;     // 42mm 弹丸允许发弹量
            uint16_t remaining_gold_coin;           // 剩余金币数量
            uint16_t projectile_allowance_fortress; // 堡垒增益点提供的储备 17mm 弹丸允许发弹量 (该值与机器人是否实际占领堡垒无关)
        } __attribute__((packed));

        // 0x0209 机器人 RFID 模块状态, 固定以 3Hz 频率发送, 服务器->己方装有 RFID 模块的机器人
        struct rfid_status_t {
            uint32_t rfid_status;
            uint8_t rfid_status_2;
        } __attribute__((packed));

        // 0x020a 飞镖选手端指令数据, 固定以 3Hz 频率发送, 服务器->己方飞镖机器人
        struct dart_client_cmd_t {
            uint8_t dart_launch_opening_status;     // 当前飞镖发射站的状态
            uint8_t reserved;
            uint16_t target_change_time;            // 切换击打目标时的比赛剩余时间, 单位：秒, 无/未切换动作, 默认为 0
            uint16_t latest_launch_cmd_time;        // 最后一次操作手确定发射指令时的比赛剩余时间, 单位：秒, 初始值为 0
        } __attribute__((packed));

        // 0x020b 地面机器人位置数据, 固定以 1Hz 频率发送, 服务器->己方哨兵机器人
        struct ground_robot_position_t {
            float hero_x;                           // 己方英雄机器人位置 x 轴坐标, 单位：m
            float hero_y;                           // 己方英雄机器人位置 y 轴坐标, 单位：m
            float engineer_x;                       // 己方工程机器人位置 x 轴坐标, 单位：m
            float engineer_y;                       // 己方工程机器人位置 y 轴坐标, 单位：m
            float standard_3_x;                     // 己方 3 号步兵机器人位置 x 轴坐标, 单位：m
            float standard_3_y;                     // 己方 3 号步兵机器人位置 y 轴坐标, 单位：m
            float standard_4_x;                     // 己方 4 号步兵机器人位置 x 轴坐标, 单位：m
            float standard_4_y;                     // 己方 4 号步兵机器人位置 y 轴坐标, 单位：m
            float reserved_1;
            float reserved_2;
        } __attribute__((packed));

        // 0x020c 雷达标记进度数据, 固定以 1Hz 频率发送, 服务器->己方雷达机器人
        struct radar_mark_data_t {
            uint16_t mark_progress;                 // 含义见文档
        } __attribute__((packed));

        // 0x020d 哨兵自主决策信息同步, 固定以 1Hz 频率发送, 服务器->己方哨兵机器人
        struct sentry_info_t {
            uint32_t sentry_info;
            uint16_t sentry_info_2;
        } __attribute__((packed));

        // 0x020e 雷达自主决策信息同步, 固定以 1Hz 频率发送, 服务器->己方雷达机器人
        struct radar_info_t {
            uint8_t radar_info;
        } __attribute__((packed));
    }

    namespace image {

    }

    namespace image::rc {
        struct raw_frame_t {
            uint16_t header;                // 0x53A9,固定帧头
            uint16_t ch0 : 11;              // 右摇杆水平位置 [364, 1684], mid = 1024
            uint16_t ch1 : 11;              // 右摇杆竖直位置 [364, 1684], mid = 1024
            uint16_t ch2 : 11;              // 左摇杆竖直位置 [364, 1684], mid = 1024
            uint16_t ch3 : 11;              // 左摇杆水平位置 [364, 1684], mid = 1024
            uint8_t sw : 2;                 // 挡位切换开关,C:0, N:1, S:2
            uint8_t key_suspend : 1;        // 暂停按键 0/1
            uint8_t key_l : 1;              // 自定义按键（左） 0/1
            uint8_t key_r : 1;              // 自定义按键（右） 0/1
            uint16_t dial : 11;             // 左上角拨轮 [364, 1684], mid = 1024
            uint8_t key_shoot: 1;           // 右上角扳机键 0/1

            int16_t mouse_x, mouse_y;       // 鼠标左右/前后移动速度,右/前为正方向
            int16_t mouse_z;                // 鼠标滚轮移动速度,向前滚动为正
            uint8_t mouse_l : 2;            // 鼠标左键 0/1
            uint8_t mouse_r : 2;            // 鼠标右键 0/1
            uint8_t mouse_m : 2;            // 鼠标中键 0/1
            uint16_t keyboard;

            uint16_t crc;
        } __attribute__((packed));
    }
}
