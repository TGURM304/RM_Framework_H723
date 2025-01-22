//
// Created by fish on 2025/1/22.
//

#ifndef BSP_RNG_H
#define BSP_RNG_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * 在 [l, r] 区间内生成随机数
 * @param l 左边界
 * @param r 有边界
 */
int bsp_rng_random(int l, int r);

#ifdef __cplusplus
}
#endif

#endif //BSP_RNG_H
