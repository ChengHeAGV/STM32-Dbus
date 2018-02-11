#include "led.h"
//LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOD的时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9); //GPIOB 高电平
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //使能GPIOD的时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //上拉输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOG,GPIO_Pin_5|GPIO_Pin_6); //GPIOG 高电平
}
