//
// Created by fish on 2024/12/13.
//

#include "bsp_tim.h"

#include "bsp_def.h"
#include "math.h"

/*
 *  For STM32H723VGT6
 *  注意时钟总线主频
 */

#define RCC_APB1_CLOCK_FREQ_MHZ 240
#define RCC_APB2_CLOCK_FREQ_MHZ 240
/**
 * @brief  设置定时器的周期和预分频
 * @param  h 定时器句柄
 * @param  period 自动重装载值
 * @param  prescaler 预分频系数
 */
void bsp_tim_set(TIM_HandleTypeDef *h, uint32_t period, uint32_t prescaler) {
    HAL_TIM_Base_DeInit(h);
    h->Init.Period    = period;
    h->Init.Prescaler = prescaler;
    HAL_TIM_Base_Init(h);
}
/**
 * @brief  根据目标频率配置定时器
 * @param  h 定时器句柄
 * @param  p 目标频率 (Hz)
 * @note   会自动计算合适的 period 和 prescaler
 *
 * -usage: bsp_tim_config(&htim1, 1000);
 */
void bsp_tim_config(TIM_HandleTypeDef *h, double p) {
    double t = 1e6 / p, k = 1, period_limit = 0;
    if(h->Instance == TIM1 || h->Instance == TIM8 || h->Instance == TIM15 || h->Instance == TIM16 ||
       h->Instance == TIM17) {
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
    bsp_tim_set(h, (uint32_t)(t / k) - 1, (uint32_t)k - 1);
}
/**
 * @brief  设置 PWM 占空比
 * @param  h 定时器句柄
 * @param  channel 定时器通道
 * @param  blank 占空比 (0~1)
 *
 * @note   0 <= blank <= 1
 *
 * -usage: bsp_tim_set_blank(&htim1, TIM_CHANNEL_1, 0.5);
 */
void bsp_tim_set_blank(TIM_HandleTypeDef *h, uint32_t channel, double blank) {
    BSP_ASSERT(0 <= blank && blank <= 1);
    __HAL_TIM_SetCompare(h, channel, (uint32_t)((h->Init.Period + 1) * blank));
}
/**
 * @brief  使能指定通道的 PWM 输出
 */
void bsp_tim_pwm_enable(TIM_HandleTypeDef *h, uint32_t channel) {
    HAL_TIM_PWM_Start(h, channel);
}
/**
 * @brief  禁用指定通道的 PWM 输出
 */
void bsp_tim_pwm_disable(TIM_HandleTypeDef *h, uint32_t channel) {
    HAL_TIM_PWM_Stop(h, channel);
}
