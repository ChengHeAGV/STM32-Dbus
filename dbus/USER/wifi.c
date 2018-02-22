void clear_rxBuf2()
{
	u16 i=0;
	for(i=0;i<USART2_REC_LEN;i++)
			USART2_RX_BUF[i]=	0;
}
//src:指令
//dst:返回信息
//timeout:超时时间
int check(char* dst,u16 timeout,char* src,...)
{
	u16 num=0;
	char *resault;
	
	u16 i;
	va_list ap;
	va_start(ap,src);
	vsprintf((char*)USART2_TX_BUF,src,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);//此次发送数据的长度
	Uart2_DMA_Tx(i);

	while(!resault&&num<(timeout/10.0))
	{
		resault=strstr(USART2_RX_BUF,dst);
		delay_ms(10);
		num++;
	}
	clear_rxBuf2();
	if(resault)
	{
		return 1;
	}
	else
		return 0;
}
