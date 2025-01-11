//
// Created by fish on 2024/9/2.
//

#ifndef BSP_USB_H
#define BSP_USB_H

#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "stdio.h"

void bsp_usb_send(uint8_t *s, uint16_t l);
void bsp_usb_printf(char *fmt, ...);

#endif //BSP_USB_H
