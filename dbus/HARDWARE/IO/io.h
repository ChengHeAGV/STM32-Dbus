#ifndef _IO_H
#define _IO_H
#include "sys.h"
//out端口定义
#define OUT1 PEout(0)
#define OUT2 PEout(1)
#define OUT3 PEout(2)
#define OUT4 PEout(3)
#define OUT5 PEout(4)
#define OUT6 PEout(5)
#define OUT7 PEout(6)
#define OUT8 PEout(7)
#define OUT9 PEout(8)
#define OUT10 PEout(9)
#define OUT11 PEout(10)
#define OUT12 PEout(11)
#define OUT13 PEout(12)
#define OUT14 PEout(13)
#define OUT15 PEout(14)
#define OUT16 PEout(15)

//input端口定义
#define IN1 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8)
#define IN2 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_9)	
#define IN3 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_10)
#define IN4 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11)
#define IN5 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_12)
#define IN6 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_13)
#define IN7 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_14)
#define IN8 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_15)

#define IN9 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_0)
#define IN10 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1)
#define IN11 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2)
#define IN12 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3)
#define IN13 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4)
#define IN14 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7)
#define IN15 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8)
#define IN16 		GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_9)
                                                     
#define IN17 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0)
#define IN18 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_1)
#define IN19 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_2)
#define IN20 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_3)
#define IN21 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_4)
#define IN22 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_5)
#define IN23 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_6)
#define IN24 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_7)                                                     
                                                    
void IO_Init(void);  //初始化
#endif





























