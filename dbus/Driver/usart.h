#ifndef __USART_H
#define __USART_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f4xx_conf.h"

void COM1Init(u32 BaudRate);
     void put_char(int ch);
#ifdef __cplusplus
}
#endif
#endif

