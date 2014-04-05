#ifndef __LED_H__
#define __LED_H__

#include "common.h"

void led_init(void);
void led_ctrl(uint32 on_off);
void led_toggle(void);

#endif