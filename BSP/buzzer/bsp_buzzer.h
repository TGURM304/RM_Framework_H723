//
// Created by fish on 2024/12/13.
//

#ifndef BSP_BUZZER_H
#define BSP_BUZZER_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void bsp_buzzer_init();
void bsp_buzzer_alarm(double freq, double blank);
void bsp_buzzer_quiet();
void bsp_buzzer_flash(double freq, double blank, uint32_t duration);

#ifdef __cplusplus
}
#endif

#endif //BSP_BUZZER_H
