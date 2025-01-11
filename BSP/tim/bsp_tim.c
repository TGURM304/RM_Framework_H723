//
// Created by fish on 2024/12/13.
//

#include "bsp_tim.h"

#include "bsp_def.h"
#include "math.h"

/*
 *  For STM32H723VGT6
 */

#define RCC_APB1_CLOCK_FREQ_MHZ 240
#define RCC_APB2_CLOCK_FREQ_MHZ 240

void bsp_tim_set(TIM_HandleTypeDef *h, uint32_t period, uint32_t prescaler) {
    HAL_TIM_Base_DeInit(h);
    h->Init.Period = period;
    h->Init.Prescaler = prescaler;
    HAL_TIM_Base_Init(h);
}

// usage: bsp_tim_config(&htim1, 1000);
void bsp_tim_config(TIM_HandleTypeDef *h, double p) {
    double t = 1e6 / p, k = 1, period_limit = 0;
    if(h->Instance == TIM1 || h->Instance == TIM8 || h->Instance == TIM15 || h->Instance == TIM16 || h->Instance == TIM17) {
        t *= RCC_APB2_CLOCK_FREQ_MHZ;
    } else {
        t *= RCC_APB1_CLOCK_FREQ_MHZ;
    }
    if(h->Instance == TIM2 || h->Instance == TIM5 || h->Instance == TIM23 || h->Instance == TIM24) {
        period_limit = (1ll << 32);
    } else {
        period_limit = (1ll << 16);
    }
    k = ceil(t / period_limit);
    bsp_tim_set(h, (uint32_t) (t / k) - 1, (uint32_t) k - 1);
}

// 0 <= blank <= 1
// usage: bsp_tim_set_blank(&htim1, TIM_CHANNEL_1, 0.5);
void bsp_tim_set_blank(TIM_HandleTypeDef *h, uint32_t channel, double blank) {
    BSP_ASSERT(0 <= blank && blank <= 1);
    __HAL_TIM_SetCompare(h, channel, (uint16_t) ((h->Init.Period + 1) * blank));
}