#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "timer.h"
#include "modbus.h"
#include "dbus.h"


//���ڷ��ͻ����� 	
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
#ifdef USART2_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
u8 TxCounter2=0;
u8 RxCounter2=0;

//ͨ���жϽ�������2���ַ�֮���ʱ������100ms�������ǲ���һ������������.
//���2���ַ����ռ������100ms,����Ϊ����1����������.Ҳ���ǳ���100msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 USART2_RX_STA=0;   	 
void USART2_IRQHandler(void)
{
//  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)                   //�����Ϊ�˱���STM32 USART ��һ���ֽڷ�����ȥ��BUG 
//  {  
//    char c=USART_ReceiveData(USART2);
//    Dbus.InPut(c);
//    USART_ClearFlag(USART2, USART_IT_RXNE);
//  }	
//	comm_END=2;                  //9600�Ĳ������£��ȴ�3.5���ֽ���ҪԼ4ms��ʱ��
//	Reciver_bit=1;              //����һ���ж���һ��λ
//	USART2_RX_BUF[RxCounter2++]= USART_ReceiveData(USART2);   //�����Ĵ��������ݻ��浽���ջ�������
//  	
//  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)                   //�����Ϊ�˱���STM32 USART ��һ���ֽڷ�����ȥ��BUG 
//  { 
//     USART_ITConfig(USART2, USART_IT_TXE, DISABLE);					     //��ֹ�����������жϣ� 
//	}										 
}  
#endif	
//��ʼ��IO ����2
//bound:������	  
void usart2_init(u32 bound)
{  
  //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��PA9��PA10

  //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���1 
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
	
//#if EN_USART2_RX	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

//����2,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);//�˴η������ݵĳ���
	for(j=0;j<i;j++)//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);  //�ȴ��ϴδ������ 
		USART_SendData(USART2,(uint8_t)USART2_TX_BUF[j]); 	 //�������ݵ�����2 
	}
}

void USART_OUT(USART_TypeDef* USARTx, char *Data,u16 len)
{ 
	while(len--)
	{				                          //�ж��Ƿ񵽴��ַ���������
		USART_SendData(USARTx, *Data++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	 }
}






