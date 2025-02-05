//
// Created by fish on 2025/1/20.
//

#pragma once

#include <functional>
#include <string>

#define TERMINAL_PORT E_UART_DEBUG

#define ANSI_CSI(code) "\033[" #code "m" /* ANSI CSI指令 */
#define ANSI_BOLD_ON      "\x1b[1m"
#define ANSI_BOLD_OFF     "\x1b[22m"

/**
 * 终端字体颜色代码
 */
#define TERMINAL_COLOR_BLACK ANSI_CSI(30)     /* 黑色 */
#define TERMINAL_COLOR_RED ANSI_CSI(31)       /* 红色 */
#define TERMINAL_COLOR_GREEN ANSI_CSI(32)     /* 绿色 */
#define TERMINAL_COLOR_YELLOW ANSI_CSI(33)    /* 黄色 */
#define TERMINAL_COLOR_BLUE ANSI_CSI(34)      /* 蓝色 */
#define TERMINAL_COLOR_FUCHSIN ANSI_CSI(35)   /* 品红 */
#define TERMINAL_COLOR_CYAN ANSI_CSI(36)      /* 青色 */
#define TERMINAL_COLOR_WHITE ANSI_CSI(37)     /* 白色 */
#define TERMINAL_COLOR_BLACK_L ANSI_CSI(90)   /* 亮黑 */
#define TERMINAL_COLOR_RED_L ANSI_CSI(91)     /* 亮红 */
#define TERMINAL_COLOR_GREEN_L ANSI_CSI(92)   /* 亮绿 */
#define TERMINAL_COLOR_YELLOW_L ANSI_CSI(93)  /* 亮黄 */
#define TERMINAL_COLOR_BLUE_L ANSI_CSI(94)    /* 亮蓝 */
#define TERMINAL_COLOR_FUCHSIN_L ANSI_CSI(95) /* 亮品红 */
#define TERMINAL_COLOR_CYAN_L ANSI_CSI(96)    /* 亮青 */
#define TERMINAL_COLOR_WHITE_L ANSI_CSI(97)   /* 亮白 */
#define TERMINAL_COLOR_DEFAULT ANSI_CSI(39)   /* 默认 */

#define TERMINAL_SEND(val, sz) bsp_uart_send(TERMINAL_PORT, (uint8_t *) val, sz)
#define TERMINAL_INFO(str) bsp_uart_printf(TERMINAL_PORT, str)
#define TERMINAL_INFO_PRINTF(str, ...) bsp_uart_printf(TERMINAL_PORT, str, __VA_ARGS__)
#define TERMINAL_ERROR(str) bsp_uart_printf(TERMINAL_PORT, TERMINAL_COLOR_RED str)
#define TERMINAL_ERROR_PRINTF(str, ...) bsp_uart_printf(TERMINAL_PORT, TERMINAL_COLOR_RED str, __VA_ARGS__)
#define TERMINAL_ERROR_BLOD(str) bsp_uart_printf(TERMINAL_PORT, ANSI_BOLD_ON TERMINAL_COLOR_RED str ANSI_BOLD_OFF)
#define TERMINAL_ERROR_BLOD_PRINTF(str, ...) bsp_uart_printf(TERMINAL_PORT, ANSI_BOLD_ON TERMINAL_COLOR_RED str ANSI_BOLD_OFF, __VA_ARGS__)

static const char TERMINAL_CLEAR_ALL[] = "\033[2J\033[1H";
static const char TERMINAL_CLEAR_LINE[] = "\033[2K\r";
static const char TERMINAL_CLEAR_BEHIND[] = "\033[K";
static const char KEY_RIGHT[] = "\033[C";
static const char KEY_LEFT[] = "\033[D";
static const char KEY_SAVE[] = "\033[s";
static const char KEY_LOAD[] = "\033[u";

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

void app_terminal_init();
void app_terminal_register_cmd(const std::string& name, const std::function <bool(std::vector<std::string>)>& func);
void app_terminal_register_cmd(const std::string& name, const std::string &brief, const std::function <bool(std::vector<std::string>)>& func);
bool* app_terminal_running_flag();