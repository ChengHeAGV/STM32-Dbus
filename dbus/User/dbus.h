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
		
	#define u8  unsigned char
	#define u16 unsigned short

	/*--- 用户配置 ---------------------------------------------------------------*/
	//读写寄存器长度
	#define DBUS_REGISTER_LENGTH 128
	//单帧数据帧最大长度
	#define DBUS_MAX_LENGTH 269
	//数据接收缓冲池长度
	#define DBUS_MAX_RECIVE_BUF 512
	//响应消息队列缓冲池长度
	#define DBUS_MAX_RESPONSE_BUF 20
	//超时时间(单位:10ms)
	#define DBUS_TIMEOUT 100
	//重发次数
	#define DBUS_MAX_REPEAT_NUM 3

	struct ReturnMsg
	{
		u8 resault;
		u16 Data;
		u16* DataBuf;
	};
    
    //寄存器
    extern u16 Dbus_Register[DBUS_REGISTER_LENGTH];

    
    
	//数据接收缓冲池
    extern char DBUS_RECIVE_BUF[DBUS_MAX_RECIVE_BUF];
    //接收缓冲池中数据长度（当前长度）
    extern u16 DBUS_RECIVE_LEN;
    //帧尾
    extern char DBUS_END;
	//初始化
	extern void Dbus_Init(u16 Address);
	extern void Heart(u16 TargetAddress);
	//输入数据
	extern void InPut(char c);
	//解包函数
	extern void OpenBox(void);
	//解析函数
	extern void Analyze(char *buf ,u8 len);
	//输出数据中断
	extern void OutPut_interrupt(void (*callback_fun)(char*,u16));
	//延时中断
	extern void Delay_interrupt(void (*callback_delay)(void));
	//心跳函数
	extern void Heart(u16 TargetAddress);
	//写单个寄存器
	extern u8 Write_Register(u16 TargetAddress,u16 RegisterAddress,u16 Data);
	//写多个寄存器
	extern u8 Write_Multiple_Registers(u16 TargetAddress,u16 RegisterAddress,u8 Num,u16* Data);	
	//读单个寄存器
	extern struct ReturnMsg Read_Register(u16 TargetAddress,u16 RegisterAddress);
	//读多个寄存器
	extern struct ReturnMsg Read_Multiple_Registers(u16 TargetAddress,u16 RegisterAddress,u8 Num);	
	
	//响应读单个寄存器
	void Response_Read_Register(char *buf);
	//响应写单个寄存器
	void Response_Write_Register(char *buf);
	//响应读多个寄存器
	void Response_Read_Multiple_Registers(char *buf);
	//响应写多个寄存器
	void Response_Write_Multiple_Registers(char *buf);
#endif
