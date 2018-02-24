/*
	******************************************************************************
  * @file    dbus.cpp
  * @author  ������
  * @version V1.2
  * @date    2018/02/01
  * @brief   �ֲ�ʽͨ��ϵͳ
  ******************************************************************************
  * @attention
  * �ֲ�ʽͨ��ϵͳ��������ƣ��豸��������ʣ���ֲֻ��Ҫ�ṩһ���������ݷ�������
  * �Ұ��յ�����д��ϵͳ�ṩ�Ľӿں������ɣ����������κ�Ӳ��
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

	/*--- �û����� ---------------------------------------------------------------*/
	//��д�Ĵ�������
	#define DBUS_REGISTER_LENGTH 128
	//��֡����֡��󳤶�
	#define DBUS_MAX_LENGTH 269
	//���ݽ��ջ���س���
	#define DBUS_MAX_RECIVE_BUF 512
	//��Ӧ��Ϣ���л���س���
	#define DBUS_MAX_RESPONSE_BUF 20
	//��ʱʱ��(��λ:10ms)
	#define DBUS_TIMEOUT 100
	//�ط�����
	#define DBUS_MAX_REPEAT_NUM 3

	struct ReturnMsg
	{
		u8 resault;
		u16 Data;
		u16* DataBuf;
	};
    
    //�Ĵ���
    extern u16 Dbus_Register[DBUS_REGISTER_LENGTH];

    
    
	//���ݽ��ջ����
    extern char DBUS_RECIVE_BUF[DBUS_MAX_RECIVE_BUF];
    //���ջ���������ݳ��ȣ���ǰ���ȣ�
    extern u16 DBUS_RECIVE_LEN;
    //֡β
    extern char DBUS_END;
	//��ʼ��
	extern void Dbus_Init(u16 Address);
	extern void Heart(u16 TargetAddress);
	//��������
	extern void InPut(char c);
	//�������
	extern void OpenBox(void);
	//��������
	extern void Analyze(char *buf ,u8 len);
	//��������ж�
	extern void OutPut_interrupt(void (*callback_fun)(char*,u16));
	//��ʱ�ж�
	extern void Delay_interrupt(void (*callback_delay)(void));
	//��������
	extern void Heart(u16 TargetAddress);
	//д�����Ĵ���
	extern u8 Write_Register(u16 TargetAddress,u16 RegisterAddress,u16 Data);
	//д����Ĵ���
	extern u8 Write_Multiple_Registers(u16 TargetAddress,u16 RegisterAddress,u8 Num,u16* Data);	
	//�������Ĵ���
	extern struct ReturnMsg Read_Register(u16 TargetAddress,u16 RegisterAddress);
	//������Ĵ���
	extern struct ReturnMsg Read_Multiple_Registers(u16 TargetAddress,u16 RegisterAddress,u8 Num);	
	
	//��Ӧ�������Ĵ���
	void Response_Read_Register(char *buf);
	//��Ӧд�����Ĵ���
	void Response_Write_Register(char *buf);
	//��Ӧ������Ĵ���
	void Response_Read_Multiple_Registers(char *buf);
	//��Ӧд����Ĵ���
	void Response_Write_Multiple_Registers(char *buf);
#endif
