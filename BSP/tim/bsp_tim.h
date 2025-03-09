//
// Created by fish on 2024/12/13.
//

#ifndef BSP_TIM_H
#define BSP_TIM_H

#include "tim.h"

#ifdef __cplusplus
extern "C" {
#endif

void bsp_tim_pwm_enable(TIM_HandleTypeDef *h, uint32_t channel);
void bsp_tim_pwm_disable(TIM_HandleTypeDef *h, uint32_t channel);
void bsp_tim_set(TIM_HandleTypeDef *h, uint32_t period, uint32_t prescaler);
void bsp_tim_config(TIM_HandleTypeDef *h, double p);
void bsp_tim_set_blank(TIM_HandleTypeDef *h, uint32_t channel, double blank);

#ifdef __cplusplus
}
#endif

#endif //BSP_TIM_H
