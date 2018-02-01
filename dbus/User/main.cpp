/**
  ******************************************************************************
  * @file   : *.cpp
  * @author : shentq
  * @version: V1.2
  * @date   : 2016/08/14

  * @brief   ebox application example .
  *
  * Copyright 2016 shentq. All Rights Reserved.         
  ******************************************************************************
 */


#include "ebox.h"
#include "dbus.h"

u32 count;

#define  Head '$'
#define  End '!'

#define  NeedHead 0
#define  NeedData 1
#define  DataEnd 2

u8 State = NeedHead;
char uart2_Recive[100];//接收数组
void test()
{
	u8 c,i;
    c = uart2.receive();
    switch(State)
    {
		case NeedHead:
			if(c == Head)
			{
				dbus_recivelength = 0;
				uart2_Recive[dbus_recivelength++] = c;
				State = NeedData;
			}
			break;
		case NeedData:
			if(c == End)
			{
				uart2_Recive[dbus_recivelength++] = c;
				State = DataEnd;
				
				//接收完成,去掉协议头，重装数据
				dbus_recivelength-=2;
				for(i=0;i<dbus_recivelength;i++)
				{
					Dbus_Recive[i]=uart2_Recive[i+1];
				}
				//解析数据
				AnalyzeDbus();
				//清空数组
				memset(uart2_Recive,0,100);
				memset(Dbus_Recive,0,100);
				State = NeedHead;
			}
			else
			{
				uart2_Recive[dbus_recivelength++] = c;
			}
			break;
    }
}
void test1()
{
//    count++;
//    PB8.toggle();
}
void setup()
{
    ebox_init();
	//LED 初始化
    PB8.mode(OUTPUT_PP);
	PB9.mode(OUTPUT_PP);
	PE0.mode(OUTPUT_PP);
	PE1.mode(OUTPUT_PP);
	PB8.reset();
	PE0.reset();
	PB9.set();
	PE1.set();
	
	//串口初始化
	uart2.begin(115200);
    uart2.attach_rx_interrupt(test);
    uart2.attach_tx_interrupt(test1);
}
//LED 闪烁参数
u16 OnTime=300;
u16 OffTime=10;

void led()
{
	PB8.toggle();
	PB9.toggle();
	PE0.toggle();
	PE1.toggle();
}

int main(void)
{
	u16 i=0;
    setup();
    while(1)
    {
		//uart2.printf("uart is ok ! count = %d\r\n", count);
		Heart();
		led();
		delay_ms(100);
		Heart();
		led();
		delay_ms(100);
		for(i=0;i<10;i++)
		{
			Write_Word(4,i,random());
			led();
			//delay_ms(10);
		}
    }
}


