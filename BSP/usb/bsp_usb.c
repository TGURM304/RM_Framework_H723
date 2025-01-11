//
// Created by fish on 2024/9/2.
//

#include "bsp_usb.h"
#include "stdarg.h"

static uint8_t buf[1024];

void bsp_usb_send(uint8_t *s, uint16_t l) {
    CDC_Transmit_HS(s, l);
}

void bsp_usb_printf(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    uint16_t len = vsnprintf((char *) buf, 1024, fmt, ap);
    va_end(ap);
    bsp_usb_send(buf, len);
}