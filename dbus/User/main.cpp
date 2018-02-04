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


long int count = 0;
//数据接收缓冲池--字符串数组
char uart2_Recive[]="03$0356342382!$014caabbcc!$043467895d!$0";
//消息响应池--二维数组
char ResponseBuf[1][1];
//将16进制字符串转数值
void HexStrToDec(char* str,char* dec)
{
	u16 i = 0;
	char temp[2];
	u8 l;
	l=strlen(str);
	for(int i=0;i<l ;i++)
	{
		temp[0]=str[i+0];
		temp[1]=str[i+1];
		sscanf(temp,"%x",&dec[i/2]);
		i++;
	}
}


char buf[100];
char temp[100];//临时数组
//数据解包
void OpenBox()
{
	u8 i = 0;
	char* p;
	u8 len=0;//向前移动的长度
	//调试用，获取长度
	count=strlen(uart2_Recive);	

	//初步排除无效数据(如果第一个开始符前有数据，则清除)
	p = strchr(uart2_Recive, '$');
	if (p)
	{
		//用len记住移动前的数量
		len=count;
		//更新接收数量
		count -= p - uart2_Recive;
		//移动的数量
		len=len-count;
		//移动有效数据
		for (i = 0; i < count; i++)
		{
			uart2_Recive[i] = uart2_Recive[i + p - uart2_Recive];
		}
		//清除剩余数据
		for (i = 0; i < len; i++)
		{
			uart2_Recive[i+count] = 0;
		}
	}

	//循环搜索并解析有效数据
	while (count > 0)
	{
		if (1 == sscanf(uart2_Recive, "$%[^!]", temp))
		{
			//由于上面条件的正则不判断是否有截止符，因此需满足以下条件(包含结束符)
			if(strchr(uart2_Recive, '!'))
			{
				//将得到的数据temp转换为数值数组
				HexStrToDec(temp,buf);

				//执行解析函数
					
				//清除第一个结束符之前的数据（已经使用过及之前的无效数据）
				p = strchr(uart2_Recive, '!');
				if (p)
				{
					//用len记住移动前的数量
					len=count;
					//更新接收数量
					count -= p - uart2_Recive + 1;
					//移动的数量
					len=len-count;
					if (count > 0)
					{
						//移动有效数据
						for (i = 0; i < count; i++)
						{
							uart2_Recive[i] = uart2_Recive[i + p - uart2_Recive + 1];
						}
						//清除剩余数据
						for (i = 0; i < len; i++)
						{
							uart2_Recive[i+count] = 0;
						}
					}
					else
					{
						//避免出现负数
						count=0;
					}
				}
			}
			else
			{
				break;
			}
		}
	}

}

//串口接收
void test()
{
	//串口收到数据加入缓冲池中
	uart2_Recive[count++] = uart2.receive();
	//收到结束符触发解包方法
	if (uart2_Recive[count - 1] == '!')
	{
		OpenBox();
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
void dbus_test()
{
	char ss[] = "123410042B66";

}

int main(void)
{
	u16 i = 0;
	Dbus dbus;
	dbus.Write_Word(1,1,1);
	dbus.OutPut_interrupt(dbus_test);
	
	dbus.InPut("a");
	
	OpenBox();
	
	setup();
	while (1)
	{
		//uart2.printf("uart is ok ! count = %d\r\n", count);
		Heart();
		led();
		delay_ms(10);
		//		Heart();
		led();
		//delay_ms(100);
		for (i = 0; i < 100; i++)
		{
			Write_Word(4, i, random(0xff));
			led();
			//delay_ms(10);
		}
	}
}


