//
// Created by fish on 2025/1/22.
//

#include "bsp_rng.h"
#include "rng.h"

int bsp_rng_random(int l, int r) {
    uint32_t rng;
    HAL_RNG_GenerateRandomNumber(&hrng, &rng);
    return (int) (l + rng % (r - l + 1));
}