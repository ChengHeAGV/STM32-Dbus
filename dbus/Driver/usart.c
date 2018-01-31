#include "usart.h"
#include <stdio.h>

//初始化串口1
void COM1Init(u32 BaudRate)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
  
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART1);  
  	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART1);
    
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
  	GPIO_Init(GPIOB, &GPIO_InitStructure);    
    
    
  	USART_InitStructure.USART_BaudRate = BaudRate;//波特率设置
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits = USART_StopBits_1;
  	USART_InitStructure.USART_Parity = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
  	USART_Init(USART3, &USART_InitStructure);
  	USART_Cmd(USART3, ENABLE);
  	USART_ClearFlag(USART3, USART_FLAG_TC);
		printf("\r\n");
		printf("123123123\r\n");
		printf("123123123\r\n");
		printf("123123123\r\n");
		
}
void put_char(int ch)
{
  	USART_SendData(USART3, (u8) ch);
  	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
}
#if 1
#pragma import(__use_no_semihosting)                              
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;         
_sys_exit(int x) 
{ 
	x = x; 
} 
int fputc(int ch, FILE *f)
{
  	USART_SendData(USART3, (u8) ch);
  	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
  	return ch;
}
#endif
