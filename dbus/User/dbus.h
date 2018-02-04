/*
	******************************************************************************
  * @file    dbus.cpp
  * @author  孙毅明
  * @version V1.2
  * @date    2018/02/01
  * @brief   分布式通信系统
  ******************************************************************************
  * @attention
  * 分布式通信系统，无主设计，设备间任意访问，移植只需要提供一个发送数据方法，并
  * 且把收到的数写入系统提供的接口函数即可，无需设置任何硬件
  * 
  * 
	*
  * 
  ******************************************************************************
*/

#ifndef __DBUS_H
#define	__DBUS_H

/*--- Includes ---------------------------------------------------------------*/
//#include "ebox.h"

/*--- 用户配置 ---------------------------------------------------------------*/
#define UART_MAX_SEND_BUF 128

#define u8 char
#define u16 unsigned short

class Dbus
{
	public:
			Dbus();
			//输入数据
			void InPut(char* str);
			//输出数据中断
	    void OutPut_interrupt(void (*callback_fun)(void));
			u8 Write_Word(u16 DstAdress,u16 RegisterAdress,u16 data);
			
	private:
			u16 DstAdress;
			u16 RegisterAdress;
			u16 data;
};

extern char Dbus_Recive[100];//接收数组
extern u16 dbus_recivelength;//接收长度
void Heart(void);//心跳函数
u8 Write_Word(u16 DstAdress,u16 RegisterAdress,u16 data);//写单个寄存器
u8 Write_MultipleWord(u16 DstAdress,u16 RegisterAdress,u8 Num,u16* Data);//写单个寄存器
void AnalyzeDbus(void);   
u16 dbus_CalcCrc(char *chData,unsigned short uNo);
void responsedata(u16 DstAdress,u8 func,u8 resault);//响应帧





                           
void send2_comm(void);
void RecFuc1(void);
void RecFuc2(void);
void RecFuc3(void);
void RecFuc4(void);
void errorsend2(u8 func,u8 type);

void Send_02(u16 adress,u16 data);//写单个寄存器
	
extern u16 Dbus_Data[100];//寄存器
extern u16 DbusLocalAddress;//本机地址

int check(char* dst,u16 timeout,char* src,...);

u16 ComperStr(u16 RegisterAdress,char* str);
u16 WriteStr(u16 DstAdress,u16 RegisterAdress,char* str);


#endif
