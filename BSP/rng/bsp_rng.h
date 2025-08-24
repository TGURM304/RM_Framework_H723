//
// Created by fish on 2025/1/22.
//

#ifndef BSP_RNG_H
#define BSP_RNG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  生成一个指定范围内的随机整数
 *
 * @param  l  随机数下界（包含）
 * @param  r  随机数上界（包含）
 *
 * @return  区间 [l, r] 内的随机整数
 *
 * @note
 *  - 使用 STM32 硬件随机数生成器 (RNG) 获取 32 位随机数
 *  - 通过取模运算将随机数映射到区间 [l, r]
 *  - 结果 = l + (rng % (r - l + 1))
 */
int bsp_rng_random(int l, int r);

#ifdef __cplusplus
}
#endif

#endif //BSP_RNG_H
