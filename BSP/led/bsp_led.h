//
// Created by fish on 2024/9/2.
//

#ifndef BSP_LED_H
#define BSP_LED_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

    void bsp_led_set(uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif

#endif //BSP_LED_H
