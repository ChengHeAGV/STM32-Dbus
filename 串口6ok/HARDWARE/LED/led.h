#ifndef _LED_H
#define _LED_H
#include "sys.h"
//LED端口定义
#define LED0 PBout(8)
#define LED1 PBout(9)

#define LED2 PGout(6)
#define LED3 PGout(5)

void LED_Init(void);  //初始化
#endif
