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
#include "led.h"


//数据接收缓冲池--字符串数组
//char uart2_Recive[]="03$0356342382!$014caabbcc!$043467895d!$0";


//串口接收
void test()
{
	//串口收到数据加入缓冲池中
	//uart2_Recive[count++] = uart2.receive();
	//收到结束符触发解包方法
//	if (uart2_Recive[count - 1] == '!')
//	{
//		//OpenBox();
//	}
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
u16 OnTime = 300;
u16 OffTime = 10;

void led()
{
	PB8.toggle();
	PB9.toggle();
	PE0.toggle();
	PE1.toggle();
}


//char s[]="ttt=eee&name=aaaaaaaaaaa&pass=bbbbbgbb&usession=undefined";
//char s[] = "03$0382!$014c!$045d!$0";
char s[] = "123410042B66";
void dbus_test(char* buf)
{
	//char ss[] = "123410042B66";

}
void dbus_delay()
{
	delay_ms(1);
}
int main(void)
{
	u16 i = 0;
	Dbus dbus(1);
	dbus.Write_Register(1,1,1);
	dbus.OutPut_interrupt(dbus_test);
	dbus.Delay_interrupt(dbus_delay);
	dbus.InPut('a');
	//OpenBox();
	setup();
	while (1)
	{
		//uart2.printf("uart is ok ! count = %d\r\n", count);
		dbus.Heart(1);
		led();
		delay_ms(10);
		//		Heart();
		led();
		//delay_ms(100);
		for (i = 0; i < 100; i++)
		{
			dbus.Write_Register(4, i, random(0xff));
			led();
			//delay_ms(10);
		}
	}
}


