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
	#define DBUS_MAX_LENGTH 1024
	//数据接收缓冲池长度
	#define DBUS_MAX_RECIVE_BUF 2048
	//响应消息队列缓冲池长度
	#define DBUS_MAX_RESPONSE_BUF 10
	//超时时间(单位:10ms)
	#define DBUS_TIMEOUT 100
	//重发次数
	#define DBUS_MAX_REPEAT_NUM 3


    
    
    /*****************Public*****************/
    /*****************Private*****************/
	//初始化
    void Init(u16 Address);
	void Heart(u16 TargetAddress);
	//输入数据
	void InPut(char c);
	//解包函数
	void OpenBox(void);
	//解析函数
	void Analyze(char *buf ,u16 len);
	//输出数据中断
    void OutPut_interrupt(void (*callback_fun)(char*,u16));
	//延时中断
	void Delay_interrupt(void (*callback_delay)(void));
	//心跳函数
	void Heart(u16 TargetAddress);
	//写单个寄存器
	u8 Write_Register(u16 TargetAddress,u16 RegisterAddress,u16 Data);
	//写多个寄存器
	u8 Write_Multiple_Registers(u16 TargetAddress,u16 RegisterAddress,u16 Num,u16* Data);	
	//读单个寄存器
	struct _ReturnMsg Read_Register(u16 TargetAddress,u16 RegisterAddress);
	//读多个寄存器
	struct _ReturnMsg Read_Multiple_Registers(u16 TargetAddress,u16 RegisterAddress,u16 Num);	
    
	//写单个寄存器
	void Post_Register(u16 TargetAddress,u16 RegisterAddress,u16 Data);
	//写多个寄存器
	void Post_Multiple_Registers(u16 TargetAddress,u16 RegisterAddress,u16 Num,u16* Data);	
    
	
	//响应读单个寄存器
	void Response_Read_Register(char *buf);
	//响应写单个寄存器
	void Response_Write_Register(char *buf);
	//响应读多个寄存器
	void Response_Read_Multiple_Registers(char *buf);
	//响应写多个寄存器
	void Response_Write_Multiple_Registers(char *buf);
  

	//响应写单个寄存器
	void Response_Post_Register(char *buf);
	//响应写多个寄存器
	void Response_Post_Multiple_Registers(char *buf);

//    extern struct _ReturnMsg ReturnMsg;
    struct _ReturnMsg
    {
        u8 resault;
        u16 Data;
        u16* DataBuf;
    };
    
    struct _Dbus
    {
        //变量
        u16* Register;
        struct _ReturnMsg ReturnMsg;
        //函数
        void (*Init)(u16 Address);
        void (*InPut)(char c);
        void (*OpenBox)(void);
        void (*OutPut_interrupt)(void (*callback_fun)(char*,u16));
        void (*Delay_interrupt)(void (*callback_delay)(void));
        
        void (*Heart)(u16 TargetAddress);
        u8 (*Write_Register)(u16 TargetAddress,u16 RegisterAddress,u16 Data);
        u8 (*Write_Multiple_Registers)(u16 TargetAddress,u16 RegisterAddress,u16 Num,u16* Data);
        struct _ReturnMsg (*Read_Register)(u16 TargetAddress,u16 RegisterAddress);
        struct _ReturnMsg (*Read_Multiple_Registers)(u16 TargetAddress,u16 RegisterAddress,u16 Num);

        //实时帧（无响应）
        void (*Post_Register)(u16 TargetAddress,u16 RegisterAddress,u16 Data);
        void (*Post_Multiple_Registers)(u16 TargetAddress,u16 RegisterAddress,u16 Num,u16* Data);
    };
    extern struct _Dbus Dbus;
#endif
