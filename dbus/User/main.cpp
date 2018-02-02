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

u32 count = 0;
char uart2_Recive[100];//接收数组

//串口接收
void test()
{
	u8 i = 0;
	char temp[100];//临时数组
	char *p;
	//串口收到数据加入缓冲池中
    uart2_Recive[count++] = uart2.receive();
	
	//搜索是否有有效数据
	if (1==sscanf(uart2_Recive,"%*[^$]$%[^!]",temp)) 
	{
		//将得到的数据转换为数值数组
		//执行解析函数
		//清除第一个结束符之前的数据（已经使用过及之前的无效数据）
		p=strchr(uart2_Recive, '!');
		if (p)
		{
			for(i=0;i<count;i++)
			{
				uart2_Recive[i]=uart2_Recive[i+p-uart2_Recive];
			}
		}

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


//char s[]="ttt=eee&name=aaaaaaaaaaa&pass=bbbbbgbb&usession=undefined";
char s[]="03$0382!$014c!$045d!$0";

char t[40];


int mum=0;
int main(void)
{
	u16 i=0;
	char* temp;
	
	char str[]="hewllo world";
    char *p=strchr(str, 'w');
    if ( p )
        mum = p-str;
	
	
    setup();
    while(1)
    {
		//uart2.printf("uart is ok ! count = %d\r\n", count);
		Heart();
		led();
		delay_ms(10);
//		Heart();
		led();
		//delay_ms(100);
		for(i=0;i<100;i++)
		{
			Write_Word(4,i,random(0xff));
			led();
			//delay_ms(10);
		}
    }
}


