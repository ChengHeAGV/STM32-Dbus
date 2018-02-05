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

#define u8 char
#define u16 unsigned short

/*--- �û����� ---------------------------------------------------------------*/
//��֡����֡��󳤶�
#define DBUS_MAX_LENGTH 256
//���ݽ��ջ���س���
#define DBUS_MAX_RECIVE_BUF 1024
//��Ӧ��Ϣ���л���س���
#define DBUS_MAX_RESPONSE_BUF 20
//����ط�����
#define DBUS_MAX_REPEAT_NUM 3
class Dbus
{
	public:
		Dbus(u16 LocalAddress);
		//��������
		void InPut(char c);
		//�������
	  void OpenBox();
		//��������
	  void Analyze(char *buf ,u8 len);
		//��������ж�
		void OutPut_interrupt(void (*callback_fun)(char*));
		//��������
		void Heart(u16 TargetAddress);
		//д�����Ĵ���
		u8 Write_Register(u16 TargetAddress,u16 RegisterAddress,u16 Data);
		//д����Ĵ���
		u8 Write_Multiple_Registers(u16 TargetAddress,u16 RegisterAddress,u8 Num,u16* Data);	
	private:
		//������ַ
		u16 LocalAddress;
		u16 DBUS_RECIVE_LEN;
		//���ݽ��ջ����
		char DBUS_RECIVE_BUF[DBUS_MAX_RECIVE_BUF];
		//��Ӧ��Ϣ���л���س���
		char DBUS_RESPONSE_BUF[DBUS_MAX_RESPONSE_BUF][DBUS_MAX_LENGTH];
};

//extern char Dbus_Recive[100];//��������
//extern u16 dbus_recivelength;//���ճ���
//void Heart(void);//��������
//u8 Write_Word(u16 DstAdress,u16 RegisterAdress,u16 data);//д�����Ĵ���
//u8 Write_MultipleWord(u16 DstAdress,u16 RegisterAdress,u8 Num,u16* Data);//д�����Ĵ���
//void AnalyzeDbus(void);   
//u16 dbus_CalcCrc(char *chData,unsigned short uNo);
//void responsedata(u16 DstAdress,u8 func,u8 resault);//��Ӧ֡





                           
//void send2_comm(void);
//void RecFuc1(void);
//void RecFuc2(void);
//void RecFuc3(void);
//void RecFuc4(void);
//void errorsend2(u8 func,u8 type);

//void Send_02(u16 adress,u16 data);//д�����Ĵ���
//	
//extern u16 Dbus_Data[100];//�Ĵ���
//extern u16 DbusLocalAddress;//������ַ

//int check(char* dst,u16 timeout,char* src,...);

//u16 ComperStr(u16 RegisterAdress,char* str);
//u16 WriteStr(u16 DstAdress,u16 RegisterAdress,char* str);


#endif
