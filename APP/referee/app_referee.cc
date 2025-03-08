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

/* Init */

OS::Task ui;
void ui_task(void *args);
void callback(bsp_uart_e e, uint8_t *s, uint16_t l);
void app_referee_init() {
    bsp_uart_set_callback(E_UART_REFEREE, callback);
    ui.Create(ui_task, static_cast<void *>(nullptr), "ui", 512, OS::Task::MEDIUM);
}

/* Receive */

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
        default: break;
    }
    data.timestamp = bsp_time_get_ms();
#undef upd
}

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

const app_referee_data_t *app_referee_data() {
    return &data;
}

/* Transmit */

static uint8_t tx_buf[1024];
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

static OS::Queue <app_referee_ui_figure_t> ui_figure_queue_(25);
static OS::Queue <app_referee_ui_string_t> ui_string_queue_(25);
static app_referee_ui_figure_t ui_figure_pkg;
static app_referee_ui_string_t ui_string_pkg;
static uint8_t ui_buf[150];

// 增加
void app_referee_ui_add(
    const char *name, uint8_t figure_type, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e
) {
    app_referee_ui_figure_t pkg = {
        .operate_type = 1,
        .figure_type = figure_type,
        .layer = layer,
        .color = color,
        .details_a = a,
        .details_b = b,
        .width = width,
        .start_x = x,
        .start_y = y,
        .details_c = c,
        .details_d = d,
        .details_e = e
    };
    std::copy_n(name, std::min(sizeof(pkg.figure_name), strlen(name)), pkg.figure_name);
    ui_figure_queue_.send(pkg);
}

// 增加 STRING
void app_referee_ui_add_string(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t font_size, const char *str
) {
    app_referee_ui_string_t pkg = {
        .operate_type = 1,
        .figure_type = 7,
        .layer = layer,
        .color = color,
        .details_a = font_size,
        .details_b = std::min(sizeof(pkg.data), strlen(str)),
        .width = width,
        .start_x = x,
        .start_y = y
    };
    std::copy_n(name, std::min(sizeof(pkg.figure_name), strlen(name)), pkg.figure_name);
    std::copy_n(str, std::min(sizeof(pkg.data), strlen(str)), pkg.data);
    ui_string_queue_.send(pkg);
}

// 修改
void app_referee_ui_upd(
    const char *name, uint8_t figure_type, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e
) {
    app_referee_ui_figure_t pkg = {
        .operate_type = 2,
        .figure_type = figure_type,
        .layer = layer,
        .color = color,
        .details_a = a,
        .details_b = b,
        .width = width,
        .start_x = x,
        .start_y = y,
        .details_c = c,
        .details_d = d,
        .details_e = e
    };
    std::copy_n(name, std::min(sizeof(pkg.figure_name), strlen(name)), pkg.figure_name);
    ui_figure_queue_.send(pkg);
}

// 修改 STRING
void app_referee_ui_upd_string(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t font_size, const char *str
) {
    app_referee_ui_string_t pkg = {
        .operate_type = 2,
        .figure_type = 7,
        .layer = layer,
        .color = color,
        .details_a = font_size,
        .details_b = std::min(sizeof(pkg.data), strlen(str)),
        .width = width,
        .start_x = x,
        .start_y = y
    };
    std::copy_n(name, std::min(sizeof(pkg.figure_name), strlen(name)), pkg.figure_name);
    std::copy_n(str, std::min(sizeof(pkg.data), strlen(str)), pkg.data);
    ui_string_queue_.send(pkg);
}


// 删除
void app_referee_ui_del(
    const char *name, uint8_t layer
) {
    app_referee_ui_figure_t pkg = {
        .operate_type = 3,
        .layer = layer,
    };
    std::copy_n(name, std::min(sizeof(pkg.figure_name), strlen(name)), pkg.figure_name);
    ui_figure_queue_.send(pkg);
}

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