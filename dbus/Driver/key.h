#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx_conf.h"

typedef enum 
{  
  KEY1 = 0,
  KEY2 = 1,
  KEY3 = 2,
  KEY4 = 3,
} Key_Def;

static const u16 KEY_PIN[4]={GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_10};

void KEYInit(void);
u8 Get_Key(Key_Def key);
u8 KEY_Scan(void);

#endif

