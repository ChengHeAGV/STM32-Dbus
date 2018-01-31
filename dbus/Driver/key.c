#include "key.h"
#include "delay.h"
void EXTILine4_Config(void);
void EXTILine5_Config(void);
void EXTILine11_Config(void);
void KEYInit(void)
{

	EXTILine4_Config();
  EXTILine5_Config();
	EXTILine11_Config();
}

void EXTILine4_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable GPIOC clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Configure PC4  pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Connect EXTI Line4 to PC4 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource4 );

  /* Configure EXTI Line4 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line4 ;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line4 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void EXTILine5_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable GPIOC clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Configure PC5 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Connect EXTI Line5 to PC5 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource5 );

  /* Configure EXTI Line5 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line5 ;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line5 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void EXTILine11_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable GPIOF clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Configure PF11 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Connect EXTI Line11 to PF11 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource11 );

  /* Configure EXTI Line11 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line11 ;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI Line11 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

u8 Get_Key(Key_Def key)
{
  	if ((GPIOB->IDR&KEY_PIN[key])!=0)return 1;
  	else return 0;
}
u8 KEY_Scan(void)
{	 
	static u8 key_up=1;
	if(key_up&&(Get_Key(KEY1)==0||Get_Key(KEY2)==0||Get_Key(KEY3)==1||Get_Key(KEY4)==0))
	{
		Delay(0xFFFFF); 
		key_up=0;
		if(Get_Key(KEY1)==0)return 1;
		else if(Get_Key(KEY2)==0)return 2;
		else if(Get_Key(KEY3)==1)return 3;
		else if(Get_Key(KEY4)==0)return 4;
	}
	else if(Get_Key(KEY1)==1&&Get_Key(KEY2)==1&&Get_Key(KEY3)==0&&Get_Key(KEY4)==1)key_up=1; 	    
	return 0;
}


