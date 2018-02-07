#include "dbus.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"

//����ص�����ָ��
typedef void (*callback_fun_type)(char*);
callback_fun_type SEND_CALLBACK;
typedef void (*callback_void_type)(void);
callback_void_type DELAY_CALLBACK;

//֡ͷ
char DBUS_HEAD = '$'; 
//֡β
char DBUS_END  = '!';
//������ʽ
char* DBUS_REGEX = "$%[^!]";
//֡ID
u16 FrameID=0;

/*********�ڲ����ܺ���********************************************************************/
			
/*********CRC16У��*******************************************************************/
u16 CRC_CALC(char *chData,unsigned short uNo)
{
	u16 crc=0xffff;
	u16 i,j;
	for(i=0;i<uNo;i++)
	{
	  crc^=chData[i];
	  for(j=0;j<8;j++)
	  {
		if(crc&1)
		{
		 crc>>=1;
		 crc^=0xA001;
		}
		else
		 crc>>=1;
	  }
	}
	return (crc);
}





/*********���ͺ���******************************************************************/
void Send(char* buf,u8 len)
{
	//���ݷ�����ʱ����
	char *TX_BUF=(char *)malloc((len+2)*sizeof(char));//���嶯̬����TX_BUF[len+2];
	unsigned int *TEMP=(unsigned int *)malloc((len)*sizeof(unsigned int));
	//��Ϣͷ 
	TX_BUF[0] = DBUS_HEAD;

	//��Ч���ݲ���ת��16����ASCII��
	sscanf(buf,"%02X",TEMP);
	for(int i=0;i<len;i++)
	{
		TX_BUF[i+1]=TEMP[i];
	}
	//��Ϣβ
	TX_BUF[len+1]=DBUS_END;
	//���÷��ͻص�����
	SEND_CALLBACK(TX_BUF);
	
	//�ͷŶ�̬���ٵĿռ�
	free(TX_BUF);
	free(TEMP);
	/*Ϊ�˷�ֹҰָ�����*/
  TX_BUF = NULL;
	TEMP   = NULL;
}




/*********��16�����ַ���ת��ֵ******************************************************************/
void HexStrToDec(char* str,char* dec)
{
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


void RecFuc1(char *buf ,u8 len);
void RecFuc2(char *buf ,u8 len);
void RecFuc3(char *buf ,u8 len);
void RecFuc4(char *buf ,u8 len);

/**********��������*******************************************************************/
Dbus::Dbus(u16 LocalAddress) 
{
	//������ַ
	this->LocalAddress = LocalAddress;
}


void Dbus::InPut(char c) 
{
	//���յ�������׷�ӵ����ջ���
	DBUS_RECIVE_BUF[DBUS_RECIVE_LEN++] = c;
	//�յ������������������
	if (DBUS_RECIVE_BUF[DBUS_RECIVE_LEN - 1] == DBUS_END)
	{
		OpenBox();
	}
}


/*********�������******************************************************************/
void Dbus::OpenBox()
{
	char* p;
	//��ǰ�ƶ��ĳ���
	u8 MOVE_LEN=0;
	//��֡��ʱ����
	char temp[DBUS_MAX_LENGTH];
	//��֡ת���������
	char buf[DBUS_MAX_LENGTH];
	
	//�����ų���Ч����(�����һ����ʼ��ǰ�����ݣ������)
	p = strchr(DBUS_RECIVE_BUF,DBUS_HEAD);
	if (p)
	{
		//��MOVE_LEN��ס�ƶ�ǰ������
		MOVE_LEN = DBUS_RECIVE_LEN;
		//���½�������
		DBUS_RECIVE_LEN -= p - DBUS_RECIVE_BUF;
		//�ƶ�������
		MOVE_LEN=MOVE_LEN-DBUS_RECIVE_LEN;
		//�ƶ���Ч����
		for (int i = 0; i < DBUS_RECIVE_LEN; i++)
		{
			DBUS_RECIVE_BUF[i] = DBUS_RECIVE_BUF[i + p - DBUS_RECIVE_BUF];
		}
		//���ʣ������
		for (int i = 0; i < MOVE_LEN; i++)
		{
			DBUS_RECIVE_BUF[i+DBUS_RECIVE_LEN] = 0;
		}
	}
	
	//ѭ��������������Ч����
	while (DBUS_RECIVE_LEN > 0)
	{
		if (1 == sscanf(DBUS_RECIVE_BUF,DBUS_REGEX,temp))//
		{
			//�������������������ж��Ƿ��н�ֹ���������������������(����������)
			if(strchr(DBUS_RECIVE_BUF, DBUS_END))
			{
				//���ְ���ʮ�������ַ���tempת��Ϊ��ֵ����buf
				HexStrToDec(temp,buf);
				//ִ�н�������
				Analyze(buf,strlen(temp));
				//�����һ��������֮ǰ�����ݣ��Ѿ�ʹ�ù���֮ǰ����Ч���ݣ�
				p = strchr(DBUS_RECIVE_BUF, DBUS_END);
				if (p)
				{
					//��MOVE_LEN��ס�ƶ�ǰ������
					MOVE_LEN=DBUS_RECIVE_LEN;
					//���½�������
					DBUS_RECIVE_LEN -= p - DBUS_RECIVE_BUF + 1;
					//�ƶ�������
					MOVE_LEN=MOVE_LEN-DBUS_RECIVE_LEN;
					if (DBUS_RECIVE_LEN > 0)
					{
						//�ƶ���Ч����
						for (int i = 0; i < DBUS_RECIVE_LEN; i++)
						{
							DBUS_RECIVE_BUF[i] = DBUS_RECIVE_BUF[i + p - DBUS_RECIVE_BUF + 1];
						}
						//���ʣ������
						for (int i = 0; i < MOVE_LEN; i++)
						{
							DBUS_RECIVE_BUF[i+DBUS_RECIVE_LEN] = 0;
						}
					}
					else
					{
						//������ָ���
						DBUS_RECIVE_LEN=0;
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



/*У�鲢��������֡
 *@Return          NONE 
*/
void Dbus::Analyze(char *buf ,u8 len)
{
	u16 CRC;
	u8 C1,C2;
	
	//�ж�Ŀ���ַ�Ƿ�Ϊ����
	if(((buf[3]<<8)|buf[4]) == LocalAddress)
	{
		//CRCУ��
		CRC=CRC_CALC(buf,(len-2));  
		C1=CRC>>8; //CRC���ֽ�
		C2=CRC;    //CRC���ֽ�
		if(C1==buf[len-2]&&C2==buf[len-1])//У����ȷ
		{		
			//����
			if(buf[2]==1)//����֡
			{
				switch(buf[5])//������
				{
					case 0x01:RecFuc1(buf,len);break; //�������Ĵ���
					case 0x02:RecFuc2(buf,len);break; //д�����Ĵ���				      
					case 0x03:RecFuc3(buf,len);break; //������Ĵ���
					case 0x04:RecFuc4(buf,len);break; //д����Ĵ���
					default:					break;  	
				}	
			}  
			else 
			if(buf[2]==2)//��Ӧ֡
			{
				//��ӵ���Ӧ֡�����
				for(int i=0;i<DBUS_MAX_RESPONSE_BUF;i++)
				{
					//����Ӧ֡������е���Ӧ֡�����
					if(DBUS_RESPONSE_BUF[i][0]==0)
					{
						//����ص�1�ֽ�Ϊ֡����
						DBUS_RESPONSE_BUF[i][0] = len;
						//������Ӧ֡���뻺���
						for(int j=0;i<len;j++)
							DBUS_RESPONSE_BUF[i][j+1] = buf[j];
					}
				}
			}
		} 
		
	}
}


/*�����жϺ�������
 *@Function        ��ʼ�������жϺ���
 *@callback_fun    Ŀ�꺯��
 *@Return          NONE 
*/
void Dbus::OutPut_interrupt(void (*callback_fun)(char*))
{
	SEND_CALLBACK = callback_fun;
}

/*��ʱ�жϺ�������
 *@callback_fun    Ŀ�꺯��
 *@Return          NONE 
*/
void Dbus::Delay_interrupt(void (*callback_delay)(void))
{
	DELAY_CALLBACK = callback_delay;
}

/*д�����Ĵ���
 *@Function        �������Ĵ���дֵ
 *@TargetAdress    Ŀ���豸��ַ
 *@RegisterAddress Ŀ��Ĵ�����ַ
 *@Data            ��д����ֵ
 *@Return          1:д��ɹ���0:д��ʧ��  
*/
u8 Dbus::Write_Register(u16 TargetAddress,u16 RegisterAddress,u16 Data)//д�����Ĵ���
{
	u16 CRC=0;	
	char TX_BUF[14];
	
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//֡ID��
	TX_BUF[1] = frameid;//֡ID��	
	TX_BUF[2] = LocalAddress>>8;//������ַ��
	TX_BUF[3] = LocalAddress;//������ַ��
	TX_BUF[4] = 1;//֡����
	TX_BUF[5] = TargetAddress>>8;//Ŀ���ַ��
	TX_BUF[6] = TargetAddress;//Ŀ���ַ��
	TX_BUF[7] = 2;//������
	TX_BUF[8] = RegisterAddress>>8;//�Ĵ�����ַ��
	TX_BUF[9] = RegisterAddress;//�Ĵ�����ַ��
	TX_BUF[10] = Data>>8;//���ݸ�
	TX_BUF[11] = Data;//���ݵ�
 
	CRC=CRC_CALC(TX_BUF,12);
	
	TX_BUF[12] = CRC>>8;//CRC��
	TX_BUF[13] = CRC;//CRC��	
		
	for(j=0;j<DBUS_MAX_REPEAT_NUM; j++)
	{
		//��������
		Send(TX_BUF,14);
		//�ȴ���Ӧ
		for(int k=0;k<DBUS_TIMEOUT;k++)
		{
			for(int i=0;i<DBUS_MAX_RESPONSE_BUF ;i++)
			{
				if(DBUS_RESPONSE_BUF[i][0]!=0)
				{
					if(((DBUS_RESPONSE_BUF[i][1]<<8)|DBUS_RESPONSE_BUF[i][2]) == frameid)
					{
						DBUS_RESPONSE_BUF[i][0]=0;
						return 1;
					}
				}
			}
			//��ʱ1ms����ֹʵʱϵͳ����ʱ����
			DELAY_CALLBACK();
		}
	}
	return 0;
}




/*��������
 *@Function        ��ʱ�㱨�豸����
 *@TargetAdress    Ŀ���豸��     
 *@Return          1:�㱨�ɹ���0:�㱨ʧ��  
*/
void Dbus::Heart(u16 TargetAddress)//��������
{
	//���ݷ�����ʱ����
	char TX_BUF[7];
	//�洢CRC��������ʱ����
	u16 CRC;
	TX_BUF[0] = LocalAddress >>8;//������ַ��
	TX_BUF[1] = LocalAddress;//������ַ��
	TX_BUF[2] = 0;//֡����
	TX_BUF[3] = 0;//Ŀ���ַ��
	TX_BUF[4] = 1;//Ŀ���ַ��
	
	CRC=CRC_CALC(TX_BUF,5);
	
	TX_BUF[5] = CRC>>8;//CRC��
	TX_BUF[6] = CRC;   //CRC��
	
	Send(TX_BUF,7);
}













/*д����Ĵ���
 *@TargetAdress    Ŀ���豸��ַ
 *@RegisterAddress Ŀ��Ĵ�����ַ
 *@Num             ��д��Ĵ�������
 *@Data            ��д����ֵָ��
 *@Return          1:д��ɹ���0:д��ʧ��  
*/
u8 Write_MultipleWord(u16 TargetAdress,u16 RegisterAddress,u8 Num,u16* Data)//д�����Ĵ���
{
	u16 CRC=0;
	char TX_BUF[14];
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//֡ID��
	TX_BUF[1] = frameid;//֡ID��		
	TX_BUF[2] = DbusLocalAddress>>8;//������ַ��
	TX_BUF[3] = DbusLocalAddress;//������ַ��
	TX_BUF[4] = 1;//֡����
	TX_BUF[5] = TargetAdress>>8;//Ŀ���ַ��
	TX_BUF[6] = TargetAdress;//Ŀ���ַ��
	TX_BUF[7] = 4;//������
	TX_BUF[8] = RegisterAddress>>8;//�Ĵ�����ַ��
	TX_BUF[9] = RegisterAddress;//�Ĵ�����ַ��
	TX_BUF[10] = Num;//�Ĵ�������

	//ѭ��д�����ݵ����ͻ�����
	for(i=0;i<Num;i++)
	{
		TX_BUF[11+2*i] = Data[i]>>8;//���ݸ�
		TX_BUF[12+2*i] = Data[i];//���ݵ�
	}

	CRC=CRC_CALC(TX_BUF,11+2*Num);

	TX_BUF[8+2*Num+1] = CRC>>8;//CRC��
	TX_BUF[8+2*Num+2] = CRC;//CRC��

	uart.printf_length(TX_BUF,9+2*Num+2);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	for(j=0;j<repeatNum; j++)
	{


		//�ȴ���Ӧ
		timeout=0;
		while(response==0&&(timeout<responseTime/10))//
		{
			timeout++;
			delay_ms(10);
		}
		if(timeout<(responseTime/10))//������Ӧ������
		{
			j = repeatNum;
			response = 0;		
			return 1;
		}
		
		//���ط�������һ
		num++;
		
		if(timeout<(responseTime/10))//��ʱ
		{
			if(num==repeatNum)//�ط������������ޣ�����ʧ�ܣ�����0
			{
				return 0;
			}
		}
	}
	return 1;
}






/*�������Ĵ���*/ 
void RecFuc1(char *buf ,u8 len)
 {

 } 
	  

/*д�����Ĵ���*/ 
void RecFuc2(char *buf ,u8 len)
 {
//	 u16 startadd;//��д��Ĵ�����ַ
//	 u16 data;//��д������
//	 
//	 startadd = (Dbus_Recive[6]<<8)|Dbus_Recive[7];
//	 data = (Dbus_Recive[8]<<8)|Dbus_Recive[9];
//	 //��������
//	 Dbus_Data[startadd] = data;
//	 	 
//	 //�ظ���Ӧ֡
//	 responsedata((Dbus_Recive[0]<<8)|Dbus_Recive[1],Dbus_Recive[5],1);
 } 
	


/*������Ĵ���*/ 
void RecFuc3(char *buf ,u8 len)
 {

 }
 
/*д����Ĵ���*/ 
void RecFuc4(char *buf ,u8 len)
 {
//	 u16 startadd;//��д��Ĵ�����ַ
//	 u16 len;//��д�����ݳ���
//	 u16 i;
//	 startadd = (Dbus_Recive[6]<<8)|Dbus_Recive[7];
//	 len = Dbus_Recive[8];
//	 for(i=0;i<len;i++)
//	 {
//		  Dbus_Data[startadd+i] = (Dbus_Recive[9+2*i]<<8)|Dbus_Recive[10+2*i];
//	 }
//	 
//	 //�ظ���Ӧ֡
//	 responsedata((Dbus_Recive[0]<<8)|Dbus_Recive[1],Dbus_Recive[5],1);
 }  


///// <summary>
///// ��Ӧ֡
///// </summary>
///// <param name="DstAdress">Ŀ���ַ</param>
///// <param name="func">������</param>
///// <param name="Data">���</param>
//void responsedata(u16 DstAdress,u8 func,u8 resault)
//{
////	u16 crctemp=0;
////	Dbus_TX_BUF[0] = DbusLocalAddress>>8;//������ַ��
////	Dbus_TX_BUF[1] = DbusLocalAddress;//������ַ��
////	Dbus_TX_BUF[2] = 2;//֡����
////	Dbus_TX_BUF[3] = DstAdress>>8;//Ŀ���ַ��
////	Dbus_TX_BUF[4] = DstAdress;//Ŀ���ַ��
////	Dbus_TX_BUF[5] = func;//������
////	Dbus_TX_BUF[6] = resault;//���
////	 
////	crctemp=dbus_CalcCrc(Dbus_TX_BUF,7);
////	
////	Dbus_TX_BUF[7] = crctemp>>8;//CRC��
////	Dbus_TX_BUF[8] = crctemp;//CRC��
////	
////	uart.printf_length(Dbus_TX_BUF,9);
//}


////src:ָ��
////dst:������Ϣ
////timeout:��ʱʱ��
//int check(char* dst,u16 timeout,char* src,...)
//{
////	u16 num=0;
////	char *resault;
////	
////	u16 i;
////	va_list ap;
////	va_start(ap,src);
////	vsprintf((char*)Dbus_TX_BUF,src,ap);
////	va_end(ap);
////	i=strlen((const char*)Dbus_TX_BUF);//�˴η������ݵĳ���

////    uart.printf_length(Dbus_TX_BUF,i);
////	while(!resault&&num<(timeout/10.0))
////	{
////		resault=strstr(Dbus_Recive,dst);
////		delay_ms(10);
////		num++;
////	}
////	clear_rxBuf2();
////	if(resault)
////	{
////		return 1;
////	}
////	else
//		return 0;
//}

////�Ƚ��ַ�����Ŀ���ַ
//u16 ComperStr(u16 RegisterAdress,char* str)
//{
//	u16 len,i;
//	len = strlen(str);//���ݳ���
//	for(i=0;i<len;i++)
//	{
//		if(Dbus_Data[RegisterAdress+i/2]>>8 != str[i])
//		{
//			return 0;
//		}
//		if(i<(len-1))
//		{
//			if((Dbus_Data[RegisterAdress+i/2]&0xff) != str[i+1])
//			{
//				return 0;
//			}
//		}
//		i++;
//	}
//	return 1;
//}

////д�ַ�����Ŀ���ַ
//u16 WriteStr(u16 DstAdress,u16 RegisterAdress,char* str)
//{
//	u16 len,i,len2=0;
//	u16 data[100];
//	len = strlen(str);//���ݳ���
//	
//	for(i=0;i<len;i++)
//	{
//		data[len2] = str[i]<<8;
//		if(i<(len-1))
//		{
//			data[len2] |= str[i+1];
//		}
//		i++;
//		len2++;
//	}
//	return Write_MultipleWord(DstAdress,RegisterAdress,len2,data);
//}










