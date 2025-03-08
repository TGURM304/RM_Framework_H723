//
// Created by fish on 2025/3/1.
//

#pragma once

#include "app_referee_def.h"

struct app_referee_header_t {
    uint8_t sof;                // 固定 0xA5
    uint16_t data_length;       // 数据帧中 data 的长度
    uint8_t seq;                // 包序号
    uint8_t crc;                // CRC8
} __attribute__ ((packed));

void app_referee_init();
const app_referee_data_t *app_referee_data();

void app_referee_ui_add(
    const char *name, uint8_t figure_type, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e
);

// LINE
inline void app_referee_ui_add_line(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x1, uint32_t y1,
    uint32_t x2, uint32_t y2
) {
    app_referee_ui_add(name, UI_FIGURE_LINE, layer, color, width, x1, y1, 0, 0, 0, x2, y2);
}

// RECTANGLE
inline void app_referee_ui_add_rectangle(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x1, uint32_t y1,
    uint32_t x2, uint32_t y2
) {
    app_referee_ui_add(name, UI_FIGURE_RECTANGLE, layer, color, width, x1, y1, 0, 0, 0, x2, y2);
}

// CIRCLE
inline void app_referee_ui_add_circle(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t r
) {
    app_referee_ui_add(name, UI_FIGURE_CIRCLE, layer, color, width, x, y, 0, 0, r, 0, 0);
}

// OVAL
inline void app_referee_ui_add_oval(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t rx, uint32_t ry
) {
    app_referee_ui_add(name, UI_FIGURE_OVAL, layer, color, width, x, y, 0, 0, 0, rx, ry);
}

// ARC
inline void app_referee_ui_add_arc(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t al, uint32_t ar, uint32_t rx, uint32_t ry
) {
    app_referee_ui_add(name, UI_FIGURE_ARC, layer, color, width, x, y, al, ar, 0, rx, ry);
}

// FLOAT
inline void app_referee_ui_add_float(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t font_size, float val
) {
    union { int32_t n; struct { uint32_t c : 10, d : 11, e : 11; } __attribute__((packed)) s; } u = {
        .n = static_cast <int32_t> (val * 1000)
    };
    app_referee_ui_add(name, UI_FIGURE_FLOAT, layer, color, width, x, y, font_size, 0, u.s.c, u.s.d, u.s.e);
}

// INT
inline void app_referee_ui_add_int(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t font_size, int32_t val
) {
    union { int32_t n; struct { uint32_t c : 10, d : 11, e : 11; } __attribute__((packed)) s; } u = {
        .n = val
    };
    app_referee_ui_add(name, UI_FIGURE_INT, layer, color, width, x, y, font_size, 0, u.s.c, u.s.d, u.s.e);
}

// STRING
void app_referee_ui_add_string(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t font_size, const char *str
);

void app_referee_ui_upd(
    const char *name, uint8_t figure_type, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e
);

// LINE
inline void app_referee_ui_upd_line(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x1, uint32_t y1,
    uint32_t x2, uint32_t y2
) {
    app_referee_ui_upd(name, UI_FIGURE_LINE, layer, color, width, x1, y1, 0, 0, 0, x2, y2);
}

// RECTANGLE
inline void app_referee_ui_upd_rectangle(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x1, uint32_t y1,
    uint32_t x2, uint32_t y2
) {
    app_referee_ui_upd(name, UI_FIGURE_RECTANGLE, layer, color, width, x1, y1, 0, 0, 0, x2, y2);
}

// CIRCLE
inline void app_referee_ui_upd_circle(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t r
) {
    app_referee_ui_upd(name, UI_FIGURE_CIRCLE, layer, color, width, x, y, 0, 0, r, 0, 0);
}

// OVAL
inline void app_referee_ui_upd_oval(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t rx, uint32_t ry
) {
    app_referee_ui_upd(name, UI_FIGURE_OVAL, layer, color, width, x, y, 0, 0, 0, rx, ry);
}

// ARC
inline void app_referee_ui_upd_arc(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t al, uint32_t ar, uint32_t rx, uint32_t ry
) {
    app_referee_ui_add(name, UI_FIGURE_ARC, layer, color, width, x, y, al, ar, 0, rx, ry);
}

// FLOAT
inline void app_referee_ui_upd_float(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t font_size, float val
) {
    union { int32_t n; struct { uint32_t c : 10, d : 11, e : 11; } __attribute__((packed)) s; } u = {
        .n = static_cast <int32_t> (val * 1000)
    };
    app_referee_ui_upd(name, UI_FIGURE_FLOAT, layer, color, width, x, y, font_size, 0, u.s.c, u.s.d, u.s.e);
}

// INT
inline void app_referee_ui_upd_int(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t font_size, int32_t val
) {
    union { int32_t n; struct { uint32_t c : 10, d : 11, e : 11; } __attribute__((packed)) s; } u = {
        .n = val
    };
    app_referee_ui_upd(name, UI_FIGURE_INT, layer, color, width, x, y, font_size, 0, u.s.c, u.s.d, u.s.e);
}

// STRING
void app_referee_ui_upd_string(
    const char *name, uint8_t layer, uint16_t color, uint32_t width, uint32_t x, uint32_t y,
    uint32_t font_size, const char *str
);


void app_referee_ui_del(
    const char *name, uint8_t layer
);