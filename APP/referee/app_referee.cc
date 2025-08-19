//
// Created by fish on 2025/3/1.
//

#include "app_referee.h"

#include "alg_crc.h"
#include "bsp_time.h"
#include "bsp_uart.h"
#include "dev_cap.h"
#include "sys_queue.h"
#include "sys_task.h"

#include <algorithm>
#include <cstring>
#include <vector>

static app_referee_header_t header;
static app_referee_data_t data;

/* ===================== Init ===================== */

OS::Task ui;
void ui_task(void *args);
void callback(bsp_uart_e e, uint8_t *s, uint16_t l);

/**
 * @brief 初始化裁判系统模块
 *
 * - 注册 UART 回调，用于接收裁判系统数据
 * - 创建 UI 任务，用于异步处理 UI 绘制/交互请求
 */
void app_referee_init() {
    bsp_uart_set_callback(E_UART_REFEREE, callback);
    ui.Create(ui_task, static_cast<void *>(nullptr), "ui", 512, OS::Task::MEDIUM);
}

/* ===================== Receive ===================== */

/**
 * @brief 数据解析与更新
 *
 * @param s 指向 payload 起始位置（即去掉 header 后的数据段）
 *
 * 根据 cmd_id 将数据写入 app_referee_data_t，并更新时间戳。
 */
void solver(uint8_t *s) {
#define upd(x) std::copy_n(s + 2, sizeof(data.x), reinterpret_cast<uint8_t *>(&data.x))
    switch(*reinterpret_cast<uint16_t *>(s)) {
    case 0x0001: upd(game_status); break;
    case 0x0002: upd(game_result); break;
    case 0x0003: upd(game_robot_hp); break;
    case 0x0201: upd(robot_status); break;
    case 0x0202: upd(power_heat_data); break;
    case 0x0203: upd(robot_pos); break;
    case 0x0204: upd(robot_buff); break;
    case 0x0206: upd(hurt_data); break;
    case 0x0207: upd(shoot_data); break;
    case 0x0208: upd(projectile_allowance); break;
    case 0x0209: upd(rfid_status); break;
    case 0x020B: upd(ground_robot_position); break;
    case 0x020D: upd(sentry_info); break;
    case 0x0302: upd(custom_controller); data.custom_controller_timestamp = bsp_time_get_ms(); break;
    case 0x0304: upd(remote_control); break;
    default: break;
    }
    data.timestamp = bsp_time_get_ms();
#undef upd
}

/**
 * @brief 裁判系统串口回调函数
 *
 * @param e UART 通道（即 E_UART_REFEREE）
 * @param s 数据指针
 * @param l 数据长度
 *
 * - 校验 header（CRC8）
 * - 校验 payload（CRC16）
 * - 若成功，调用 solver() 解析
 */
void callback(bsp_uart_e e, uint8_t *s, uint16_t l) {
    if(l < sizeof(header)) return;
    for(size_t i = 0; i < l; i++) {
        if(s[i] != 0xA5) continue;
        if(!CRC8::verify(s + i, sizeof(header))) continue;
        std::copy_n(s + i, sizeof(header), reinterpret_cast<uint8_t *>(&header));
        if(!CRC16::verify(s + i, header.data_length + sizeof(header) + 4)) continue;
        solver(s + i + sizeof(header));
        i += header.data_length + sizeof(header) + 4 - 1;
    }
}

/**
 * @brief 获取当前裁判系统数据指针
 *
 * @return const app_referee_data_t* 指向最新数据的常量指针
 */
const app_referee_data_t *app_referee_data() {
    return &data;
}

/* ===================== Transmit ===================== */

static uint8_t tx_buf[1024];

/**
 * @brief 向裁判系统发送数据包
 *
 * @param cmd_id 指令 ID
 * @param s 数据内容指针
 * @param l 数据长度
 *
 * 自动生成 header（含 CRC8），追加数据与 CRC16 校验，并通过 UART 发送。
 */
void transmit(uint16_t cmd_id, uint8_t *s, uint16_t l) {
    // header
    app_referee_header_t pkg_header = {
        .sof = 0xA5, .data_length = l, .seq = 0, .crc = 0
    };
    CRC8::append(pkg_header); std::copy_n(reinterpret_cast<uint8_t *>(&pkg_header), sizeof(pkg_header), tx_buf);
    // cmd_id
    std::copy_n(reinterpret_cast<uint8_t *>(&cmd_id), sizeof(cmd_id), tx_buf + sizeof(pkg_header));
    // data
    std::copy_n(s, l, tx_buf + sizeof(pkg_header) + sizeof(cmd_id));
    // crc16
    uint16_t crc = CRC16::calc(tx_buf, sizeof(pkg_header) + sizeof(cmd_id) + l);
    std::copy_n(reinterpret_cast<uint8_t *>(&crc), sizeof(crc), tx_buf + sizeof(pkg_header) + sizeof(cmd_id) + l);

    bsp_uart_send(E_UART_REFEREE, tx_buf, sizeof(pkg_header) + sizeof(cmd_id) + l + sizeof(crc));
}

/* ===================== UI Func ===================== */

static OS::Queue <app_referee_ui_figure_t> ui_figure_queue_(25);
static OS::Queue <app_referee_ui_string_t> ui_string_queue_(25);
static app_referee_ui_figure_t ui_figure_pkg;
static app_referee_ui_string_t ui_string_pkg;
static uint8_t ui_buf[150];

/**
 * @brief 新增 UI 图形
 *
 * @param name 图形名称（<=3字符）
 * @param figure_type 图形类型（直线、矩形、圆等）
 * @param layer 图层
 * @param color 颜色
 * @param width 线宽
 * @param x,y 起始坐标
 * @param a-e 额外参数（依赖图形类型）
 */
void app_referee_ui_add(
    const char *name, uint8_t figure_type, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e
);

/**
 * @brief 新增 UI 文字
 *
 * @param name 名称（<=3字符）
 * @param layer 图层
 * @param color 颜色
 * @param width 线宽
 * @param x,y 起始坐标
 * @param font_size 字号
 * @param str 文字内容
 */
void app_referee_ui_add_string(...);

/**
 * @brief 修改 UI 图形
 *
 * 与 app_referee_ui_add 类似，但 operate_type = 2
 */
void app_referee_ui_upd(...);

/**
 * @brief 修改 UI 文字
 *
 * 与 app_referee_ui_add_string 类似，但 operate_type = 2
 */
void app_referee_ui_upd_string(...);

/**
 * @brief 删除 UI 元素
 *
 * @param name 元素名称
 * @param layer 所在图层
 */
void app_referee_ui_del(const char *name, uint8_t layer);

/* ===================== UI Task ===================== */

/**
 * @brief UI 任务
 *
 * - 从队列中取出待绘制的图形/文字
 * - 根据数量打包为不同的 cmd_id（0x0101~0x0104）
 * - 调用 transmit() 发送到裁判系统
 * - 每次发送后 Sleep 35ms，避免过快
 */
void ui_task(void *args) {
    uint16_t sender = 3, receiver = 0x0103;
    while(true) {
        while(!ui_figure_queue_.size() and !ui_string_queue_.size())
            OS::Task::SleepMilliseconds(1);

        if(ui_string_queue_.size()) {
            app_referee_robot_interaction_header_t ui_header = {
                .data_cmd_id = 0x0110,
                .sender_id = sender,
                .receiver_id = receiver,
            };
            std::copy_n(reinterpret_cast<uint8_t *>(&ui_header), sizeof(ui_header), ui_buf);
            ui_string_queue_.receive(ui_string_pkg);
            std::copy_n(reinterpret_cast<uint8_t *>(&ui_string_pkg), sizeof(ui_string_pkg), ui_buf + sizeof(ui_header));
            transmit(0x0301, ui_buf, sizeof(ui_header) + sizeof(ui_string_pkg));
        }
        else if(ui_figure_queue_.size() < 3) {
            app_referee_robot_interaction_header_t ui_header = {
                .data_cmd_id = 0x0101,
                .sender_id = sender,
                .receiver_id = receiver
            };
            std::copy_n(reinterpret_cast<uint8_t *>(&ui_header), sizeof(ui_header), ui_buf);
            ui_figure_queue_.receive(ui_figure_pkg);
            std::copy_n(reinterpret_cast<uint8_t *>(&ui_figure_pkg), sizeof(ui_figure_pkg), ui_buf + sizeof(ui_header));
            transmit(0x0301, ui_buf, sizeof(ui_header) + sizeof(ui_figure_pkg));
        }
        else if(ui_figure_queue_.size() < 5) {
            app_referee_robot_interaction_header_t ui_header = {
                .data_cmd_id = 0x0102,
                .sender_id = sender,
                .receiver_id = receiver
            };
            std::copy_n(reinterpret_cast<uint8_t *>(&ui_header), sizeof(ui_header), ui_buf);
            for(size_t i = 0; i < 2; i++) {
                ui_figure_queue_.receive(ui_figure_pkg);
                std::copy_n(reinterpret_cast<uint8_t *>(&ui_figure_pkg), sizeof(ui_figure_pkg), ui_buf + sizeof(ui_header) + sizeof(ui_figure_pkg) * i);
            }
            transmit(0x0301, ui_buf, sizeof(ui_header) + sizeof(ui_figure_pkg) * 2);
        }
        else if(ui_figure_queue_.size() < 7) {
            app_referee_robot_interaction_header_t ui_header = {
                .data_cmd_id = 0x0103,
                .sender_id = sender,
                .receiver_id = receiver
            };
            std::copy_n(reinterpret_cast<uint8_t *>(&ui_header), sizeof(ui_header), ui_buf);
            for(size_t i = 0; i < 5; i++) {
                ui_figure_queue_.receive(ui_figure_pkg);
                std::copy_n(reinterpret_cast<uint8_t *>(&ui_figure_pkg), sizeof(ui_figure_pkg), ui_buf + sizeof(ui_header) + sizeof(ui_figure_pkg) * i);
            }
            transmit(0x0301, ui_buf, sizeof(ui_header) + sizeof(ui_figure_pkg) * 5);
        }
        else {
            app_referee_robot_interaction_header_t ui_header = {
                .data_cmd_id = 0x0104,
                .sender_id = sender,
                .receiver_id = receiver
            };
            std::copy_n(reinterpret_cast<uint8_t *>(&ui_header), sizeof(ui_header), ui_buf);
            for(size_t i = 0; i < 7; i++) {
                ui_figure_queue_.receive(ui_figure_pkg);
                std::copy_n(reinterpret_cast<uint8_t *>(&ui_figure_pkg), sizeof(ui_figure_pkg), ui_buf + sizeof(ui_header) + sizeof(ui_figure_pkg) * i);
            }
            transmit(0x0301, ui_buf, sizeof(ui_header) + sizeof(ui_figure_pkg) * 7);
        }
        OS::Task::SleepMilliseconds(35);
    }
}
