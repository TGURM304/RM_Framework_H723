//
// Created by fish on 2024/9/2.
//

#include "bsp_led.h"
#include "bsp_def.h"
#include "spi.h"

#define WS2812_HIGH    0xf0
#define WS2812_LOW     0xc0
#define WS2812_PORT    &hspi6

static uint8_t buf[24];
/**
 * @brief  设置板载 WS2812 灯珠的 RGB 颜色
 *
 * @param  r  红色分量 (0~255)
 * @param  g  绿色分量 (0~255)
 * @param  b  蓝色分量 (0~255)
 *
 * @note   - 本函数通过 SPI 向 WS2812 灯珠发送数据，控制其发光颜色
 *         - WS2812 需要将每一位 RGB 数据编码成特定的高低电平波形，
 *           这里使用 WS2812_HIGH / WS2812_LOW 预定义的值来生成对应的波形数据
 *         - buf 数组按 WS2812 的通信协议排列，其中：
 *             - buf[0..7]   存放 G 分量的 8bit 数据
 *             - buf[8..15]  存放 R 分量的 8bit 数据
 *             - buf[16..23] 存放 B 分量的 8bit 数据
 *         - 最终通过 HAL_SPI_Transmit() 将 24bit 数据发出
 *
 * @warning WS2812 的时序要求非常严格，SPI 的波特率和 WS2812_HIGH/LOW 的值
 *          必须正确配置，否则 LED 可能无法点亮或颜色错误
 */
void bsp_led_set(uint8_t r, uint8_t g, uint8_t b) {
    for(int i = 0; i < 8; i++) {
        buf[7-i]  = (((g >> i) & 1) ? WS2812_HIGH : WS2812_LOW) >> 1;
        buf[15-i] = (((r >> i) & 1) ? WS2812_HIGH : WS2812_LOW) >> 1;
        buf[23-i] = (((b >> i) & 1) ? WS2812_HIGH : WS2812_LOW) >> 1;
    }
    HAL_SPI_Transmit(WS2812_PORT, buf, 24, HAL_MAX_DELAY);
}