#include "timer.h"
#include "led.h"
#include "usart2.h"
#include "modbus.h"

extern u8  comm_END;
extern u8  Reciver_bit;//接收位置一
extern u8 TxCounter2;
extern u8 RxCounter2;
extern uint8_t USART2_TX_BUF[]; 
extern uint8_t USART2_RX_BUF[];


extern u16 USART2_RX_STA;

//定时器7中断服务程序		    
void TIM3_IRQHandler(void)
{ 	
	 if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET ) /*检查TIM3更新中断发生与否*/
    {
       TIM_ClearITPendingBit(TIM3, TIM_IT_Update); /*清除TIMx更新中断标志 */
			 if(Reciver_bit&&comm_END!=0)
				comm_END--; 
			 if(comm_END==0)
			 {
				 Reciver_bit=0;                     //帧接收，对标志位清零	 	 
				if((USART2_RX_BUF[0]==1||USART2_RX_BUF[0]==0)&&RxCounter2>=8)
					{
						USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
						//u2_printf(USART2_RX_BUF);
						AnalyzeRecieve();
						if(USART2_RX_BUF[0]==1)
						send_comm();
						USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
					}
						comm_END=2;                                //重新赋延时初值
// 					for(i=0;i<32;i++)                            //清空接收数组
// 					USART2_RX_BUF[i]=0;		
					RxCounter2 = 0;
					USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
				}
			}
	
//	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//是更新中断
//	{	 			   
//		USART2_RX_STA|=1<<15;	//标记接收完成
//		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //清除TIM7更新中断标志    
//		TIM_Cmd(TIM7, DISABLE);  //关闭TIM7 
//	}	    
}
 
//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM3_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//TIM7时钟使能    
	
	//定时器TIM7初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断
   
	TIM_Cmd(TIM3,ENABLE);//使能定时器7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

//		TIM_TimeBaseInitTypeDef  TIM3_TimeBaseStructure;
//		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		    //定时器3 时钟使能
//		/*-------------------------------------------------------------------
//		TIM3CLK=72MHz  预分频系数Prescaler=63 经过分频 定时器时钟为1.125MHz
//		捕获/比较寄存器2 TIM3_CCR2= CCR2_Val 
//		2通道产生的更新频率是=1.125MHz/CCR2_Val=25Hz

//		-------------------------------------------------------------------*/
//		TIM3_TimeBaseStructure.TIM_Prescaler = 72;						//预分频器TIM3_PSC=63	 
//		TIM3_TimeBaseStructure.TIM_Period =0xffff;				        //自动重装载寄存器TIM3_APR      		     
//		TIM3_TimeBaseStructure.TIM_ClockDivision = 0x0;					//时钟分频因子 TIM3_CR1[9:8]=00
//		TIM3_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//计数器向上计数模式 TIM3_CR1[4]=0
//		TIM_TimeBaseInit(TIM3,&TIM3_TimeBaseStructure);					//写TIM3各寄存器参数
//		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 							//TIM3_DIER[2]=1  允许捕获/比较2中断
//		TIM_Cmd(TIM3,ENABLE);											//启动定时器3 TIM3_CR1[0]=1;  
	
}
	 
