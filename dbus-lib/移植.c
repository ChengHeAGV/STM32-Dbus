
void WIFI_Send(char *str,u16 len)
{
	//�ַ����ͺ���
	USART_OUT(USART6,str,len);
}
void delay()
{
//    OS_ERR *err;
    //OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,err);
    delay_ms(10);
}


//������
int main(void)
{
	//��ʼ������
	usart6_init(115200);
	//��ʼ��DBUS
    Dbus.Init(5);//����Ĳ����Ǳ���ID
	Dbus.OutPut_interrupt(WIFI_Send);	
	Dbus.Delay_interrupt(delay);
	
	//��ʼ���Ĵ���
	//Dbus.Register[0]=0x1122;
    //Dbus.Register[1]=0x3366;
    //Dbus.Register[3]=0xaabb;
	//д���Ĵ���
	//Write_Register(0,1,num);
	//���Ĵ���
	//Dbus.ReturnMsg = Read_Register(0,1);
}


//task ��������
void task_task(void *p_arg)
{
	//�ϵ緢һ������
	Heart(1);
	while(1)
	{
        Dbus.Heart(0);
		delay_ms(1000);		
	}
}

//task ��������
void task_task(void *p_arg)
{
	while(1)
	{
        //�յ���������������� 
        Dbus.OpenBox();
		delay_ms(10);
	}
}
void USART6_IRQHandler(void)
{
//  USART6_RX_BUF[RxCounter6++]= c;   //�����Ĵ��������ݻ��浽���ջ�������
  if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)                   //�����Ϊ�˱���STM32 USART ��һ���ֽڷ�����ȥ��BUG 
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
		//�ж��Ƿ񵽴��ַ���������
		USART_SendData(USARTx, *Data++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	 }
}



