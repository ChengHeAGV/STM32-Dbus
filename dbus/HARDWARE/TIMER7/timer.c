#include "timer.h"
#include "led.h"
#include "usart2.h"
#include "modbus.h"

extern u8  comm_END;
extern u8  Reciver_bit;//����λ��һ
extern u8 TxCounter2;
extern u8 RxCounter2;
extern uint8_t USART2_TX_BUF[]; 
extern uint8_t USART2_RX_BUF[];


extern u16 USART2_RX_STA;

//��ʱ��7�жϷ������		    
void TIM3_IRQHandler(void)
{ 	
	 if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET ) /*���TIM3�����жϷ������*/
    {
       TIM_ClearITPendingBit(TIM3, TIM_IT_Update); /*���TIMx�����жϱ�־ */
			 if(Reciver_bit&&comm_END!=0)
				comm_END--; 
			 if(comm_END==0)
			 {
				 Reciver_bit=0;                     //֡���գ��Ա�־λ����	 	 
				if((USART2_RX_BUF[0]==1||USART2_RX_BUF[0]==0)&&RxCounter2>=8)
					{
						USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
						//u2_printf(USART2_RX_BUF);
						AnalyzeRecieve();
						if(USART2_RX_BUF[0]==1)
						send_comm();
						USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
					}
						comm_END=2;                                //���¸���ʱ��ֵ
// 					for(i=0;i<32;i++)                            //��ս�������
// 					USART2_RX_BUF[i]=0;		
					RxCounter2 = 0;
					USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
				}
			}
	
//	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//�Ǹ����ж�
//	{	 			   
//		USART2_RX_STA|=1<<15;	//��ǽ������
//		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //���TIM7�����жϱ�־    
//		TIM_Cmd(TIM7, DISABLE);  //�ر�TIM7 
//	}	    
}
 
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM3_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//TIM7ʱ��ʹ��    
	
	//��ʱ��TIM7��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�
   
	TIM_Cmd(TIM3,ENABLE);//ʹ�ܶ�ʱ��7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

//		TIM_TimeBaseInitTypeDef  TIM3_TimeBaseStructure;
//		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		    //��ʱ��3 ʱ��ʹ��
//		/*-------------------------------------------------------------------
//		TIM3CLK=72MHz  Ԥ��Ƶϵ��Prescaler=63 ������Ƶ ��ʱ��ʱ��Ϊ1.125MHz
//		����/�ȽϼĴ���2 TIM3_CCR2= CCR2_Val 
//		2ͨ�������ĸ���Ƶ����=1.125MHz/CCR2_Val=25Hz

//		-------------------------------------------------------------------*/
//		TIM3_TimeBaseStructure.TIM_Prescaler = 72;						//Ԥ��Ƶ��TIM3_PSC=63	 
//		TIM3_TimeBaseStructure.TIM_Period =0xffff;				        //�Զ���װ�ؼĴ���TIM3_APR      		     
//		TIM3_TimeBaseStructure.TIM_ClockDivision = 0x0;					//ʱ�ӷ�Ƶ���� TIM3_CR1[9:8]=00
//		TIM3_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���������ϼ���ģʽ TIM3_CR1[4]=0
//		TIM_TimeBaseInit(TIM3,&TIM3_TimeBaseStructure);					//дTIM3���Ĵ�������
//		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 							//TIM3_DIER[2]=1  ������/�Ƚ�2�ж�
//		TIM_Cmd(TIM3,ENABLE);											//������ʱ��3 TIM3_CR1[0]=1;  
	
}
	 
