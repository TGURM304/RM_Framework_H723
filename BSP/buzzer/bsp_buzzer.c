//
// Created by fish on 2024/12/13.
//

#include "bsp_buzzer.h"

#include "bsp_tim.h"
#include "cmsis_os2.h"
/**
 * @brief  蜂鸣器初始化函数，开启PWM输出驱动蜂鸣器
 *
 */
void bsp_buzzer_init() {
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);
}

/**
 * @brief  使蜂鸣器发声（报警模式）
 *
 *
 * @param  freq   蜂鸣器发声频率 (Hz)，决定音调高低
 * @param  blank  PWM 占空比 (0.0 ~ 1.0)，决定音量大小
 */
void bsp_buzzer_alarm(double freq, double blank) {
    bsp_tim_config(&htim12, freq);
    bsp_tim_set_blank(&htim12, TIM_CHANNEL_2, blank);
}

/**
 * @brief  关闭蜂鸣器（静音）
 *
 * 将蜂鸣器 PWM 占空比设置为 0，立即停止发声。
 */
void bsp_buzzer_quiet() {
    bsp_tim_set_blank(&htim12, TIM_CHANNEL_2, 0);
}

/**
 * @brief  蜂鸣器鸣叫一段时间（闪烁模式）
 *
 *
 * @param  freq      蜂鸣器发声频率 (Hz)
 * @param  blank     PWM 占空比 (0.0 ~ 1.0)，决定音量
 * @param  duration  发声持续时间 (ms)
 */
void bsp_buzzer_flash(double freq, double blank, uint32_t duration) {
    bsp_buzzer_alarm(freq, blank);
    osDelay(duration);
    bsp_buzzer_quiet();
}
