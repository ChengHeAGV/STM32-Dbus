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
//���ݽ��ջ����--�ַ�������
char uart2_Recive[]="03$0356342382!$014caabbcc!$043467895d!$0";
//��Ϣ��Ӧ��--��ά����
char ResponseBuf[1][1];
//��16�����ַ���ת��ֵ
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
char temp[100];//��ʱ����
//���ݽ��
void OpenBox()
{
	u8 i = 0;
	char* p;
	u8 len=0;//��ǰ�ƶ��ĳ���
	//�����ã���ȡ����
	count=strlen(uart2_Recive);	

	//�����ų���Ч����(�����һ����ʼ��ǰ�����ݣ������)
	p = strchr(uart2_Recive, '$');
	if (p)
	{
		//��len��ס�ƶ�ǰ������
		len=count;
		//���½�������
		count -= p - uart2_Recive;
		//�ƶ�������
		len=len-count;
		//�ƶ���Ч����
		for (i = 0; i < count; i++)
		{
			uart2_Recive[i] = uart2_Recive[i + p - uart2_Recive];
		}
		//���ʣ������
		for (i = 0; i < len; i++)
		{
			uart2_Recive[i+count] = 0;
		}
	}

	//ѭ��������������Ч����
	while (count > 0)
	{
		if (1 == sscanf(uart2_Recive, "$%[^!]", temp))
		{
			//�������������������ж��Ƿ��н�ֹ���������������������(����������)
			if(strchr(uart2_Recive, '!'))
			{
				//���õ�������tempת��Ϊ��ֵ����
				HexStrToDec(temp,buf);

				//ִ�н�������
					
				//�����һ��������֮ǰ�����ݣ��Ѿ�ʹ�ù���֮ǰ����Ч���ݣ�
				p = strchr(uart2_Recive, '!');
				if (p)
				{
					//��len��ס�ƶ�ǰ������
					len=count;
					//���½�������
					count -= p - uart2_Recive + 1;
					//�ƶ�������
					len=len-count;
					if (count > 0)
					{
						//�ƶ���Ч����
						for (i = 0; i < count; i++)
						{
							uart2_Recive[i] = uart2_Recive[i + p - uart2_Recive + 1];
						}
						//���ʣ������
						for (i = 0; i < len; i++)
						{
							uart2_Recive[i+count] = 0;
						}
					}
					else
					{
						//������ָ���
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

//���ڽ���
void test()
{
	//�����յ����ݼ��뻺�����
	uart2_Recive[count++] = uart2.receive();
	//�յ������������������
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
	//LED ��ʼ��
	PB8.mode(OUTPUT_PP);
	PB9.mode(OUTPUT_PP);
	PE0.mode(OUTPUT_PP);
	PE1.mode(OUTPUT_PP);
	PB8.reset();
	PE0.reset();
	PB9.set();
	PE1.set();

	//���ڳ�ʼ��
	uart2.begin(115200);
	uart2.attach_rx_interrupt(test);
	uart2.attach_tx_interrupt(test1);
}
//LED ��˸����
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


