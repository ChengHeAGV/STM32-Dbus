#ifndef __DELAY_H
#define __DELAY_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f4xx_conf.h"

void delay_ms(u16 nms);
void delay_us(u32 nus);
void Delay(u32 count);
#ifdef __cplusplus
}
#endif
#endif

