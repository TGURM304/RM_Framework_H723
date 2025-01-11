//
// Created by fish on 2024/10/6.
//

#ifndef BSP_ADC_H
#define BSP_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

void bsp_adc_init(void);
float bsp_adc_vbus(void);

#ifdef __cplusplus
}
#endif

#endif //BSP_ADC_H
