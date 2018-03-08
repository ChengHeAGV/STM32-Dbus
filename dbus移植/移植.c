
void WIFI_Send(char *str,u16 len)
{
	//字符发送函数
	USART_OUT(USART6,str,len);
}
void delay()
{
//    OS_ERR *err;
    //OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,err);
    delay_ms(10);
}


//主函数
int main(void)
{
	//初始化串口
	usart6_init(115200);
	//初始化DBUS
    Dbus.Init(5);//这里的参数是本机ID
	Dbus.OutPut_interrupt(WIFI_Send);	
	Dbus.Delay_interrupt(delay);
	
	//初始化寄存器
	//Dbus.Register[0]=0x1122;
    //Dbus.Register[1]=0x3366;
    //Dbus.Register[3]=0xaabb;
	//写服寄存器
	//Write_Register(0,1,num);
	//读寄存器
	//Dbus.ReturnMsg = Read_Register(0,1);
}


//task 心跳任务
void task_task(void *p_arg)
{
	//上电发一次心跳
	Heart(1);
	while(1)
	{
        Dbus.Heart(0);
		delay_ms(1000);		
	}
}

//task 解析任务
void task_task(void *p_arg)
{
	while(1)
	{
        //收到结束符触发解包函 
        Dbus.OpenBox();
		delay_ms(10);
	}
}
void USART6_IRQHandler(void)
{
//  USART6_RX_BUF[RxCounter6++]= c;   //将读寄存器的数据缓存到接收缓冲区里
  if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)                   //这段是为了避免STM32 USART 第一个字节发不出去的BUG 
  {  
    char c=USART_ReceiveData(USART6);
    Dbus.InPut(c);
    USART_ClearFlag(USART6, USART_IT_RXNE);
  }	
} 

void USART_OUT(USART_TypeDef* USARTx, char *Data,u16 len)
{ 
	while(len--)
	{	
		//判断是否到达字符串结束符
		USART_SendData(USARTx, *Data++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	 }
}



