#include "led.h"
//LED IO��ʼ��
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //ʹ��GPIOD��ʱ��
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9); //GPIOB �ߵ�ƽ
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //ʹ��GPIOD��ʱ��
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOG,GPIO_Pin_5|GPIO_Pin_6); //GPIOG �ߵ�ƽ
}
