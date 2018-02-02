#include "dbus.h"
#include "string.h"
#include "stdarg.h"

u16 Dbus_Data[100];//�Ĵ���
char Dbus_Recive[100];//��������

u16 dbus_sendlength;//���ͳ���
u16 dbus_recivelength;//���ճ���
u16 responseTime=3000;//��Ӧʱ��3��
u16 repeatNum=3;//�ط�����3��
u16 DbusLocalAddress = 4;//������ַ
u16 response=0;//��Ӧ��־

char Dbus_TX_BUF[100];

Usart uart = uart2;

//�������ݵ�������
void SendToSever(char* buf,u8 len)
{
	u8 i;
	//������Ϣͷ 
	uart.printf("$");
	//��Ч���ݲ���ת��16����ASCII��
	for(int i=0;i<len;i++)
	{
		uart.printf("%02X",buf[i]);
	}
	//������Ϣβ
	uart.printf("!");
}

void Heart()//��������
{
	u16 crctemp=0;
	
	Dbus_TX_BUF[0] = DbusLocalAddress>>8;//������ַ��
	Dbus_TX_BUF[1] = DbusLocalAddress;//������ַ��
	Dbus_TX_BUF[2] = 0;//֡����
	Dbus_TX_BUF[3] = 0;//Ŀ���ַ��
	Dbus_TX_BUF[4] = 1;//Ŀ���ַ��
	
	crctemp=dbus_CalcCrc(Dbus_TX_BUF,5);
	
	Dbus_TX_BUF[5] = crctemp>>8;//CRC��
	Dbus_TX_BUF[6] = crctemp;//CRC��
	
	SendToSever(Dbus_TX_BUF,7);
}

/// <summary>
/// д�����ֵ�Ŀ���ַ
/// </summary>
/// <param name="DstAdress">Ŀ���ַ</param>
/// <param name="RegisterAdress">�Ĵ�����ַ</param>
/// <param name="Data">��д������</param>
u8 Write_Word(u16 DstAdress,u16 RegisterAdress,u16 data)//д�����Ĵ���
{
	u16 crctemp=0;
	u16 j,timeout,num=0;
	
	char BUF[12];
	
	for(j=0;j<repeatNum; j++)
	{
		BUF[0] = DbusLocalAddress>>8;//������ַ��
		BUF[1] = DbusLocalAddress;//������ַ��
		BUF[2] = 1;//֡����
		BUF[3] = DstAdress>>8;//Ŀ���ַ��
		BUF[4] = DstAdress;//Ŀ���ַ��
		BUF[5] = 2;//������
		BUF[6] = RegisterAdress>>8;//�Ĵ�����ַ��
		BUF[7] = RegisterAdress;//�Ĵ�����ַ��
		BUF[8] = data>>8;//���ݸ�
		BUF[9] = data;//���ݵ�
	 
		crctemp=dbus_CalcCrc(BUF,10);
		
		BUF[10] = crctemp>>8;//CRC��
		BUF[11] = crctemp;//CRC��
		
		SendToSever(BUF,12);
		//�ȴ���Ӧ
		timeout=0;
		while(response==0&&(timeout<responseTime/10))
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


/// <summary>
/// д����ֵ�Ŀ���ַ
/// </summary>
/// <param name="DstAdress">Ŀ���ַ</param>
/// <param name="RegisterAdress">�Ĵ�����ʼ��ַ</param>
/// <param name="Num">�Ĵ�������</param>
/// <param name="Data">��д������ָ��</param>
u8 Write_MultipleWord(u16 DstAdress,u16 RegisterAdress,u8 Num,u16* Data)//д�����Ĵ���
{
	u16 crctemp=0;
	u16 i = 0,j,timeout,num=0;
	for(j=0;j<repeatNum; j++)
	{
		Dbus_TX_BUF[0] = DbusLocalAddress>>8;//������ַ��
		Dbus_TX_BUF[1] = DbusLocalAddress;//������ַ��
		Dbus_TX_BUF[2] = 1;//֡����
		Dbus_TX_BUF[3] = DstAdress>>8;//Ŀ���ַ��
		Dbus_TX_BUF[4] = DstAdress;//Ŀ���ַ��
		Dbus_TX_BUF[5] = 4;//������
		Dbus_TX_BUF[6] = RegisterAdress>>8;//�Ĵ�����ַ��
		Dbus_TX_BUF[7] = RegisterAdress;//�Ĵ�����ַ��
		Dbus_TX_BUF[8] = Num;//�Ĵ�������

		//ѭ��д�����ݵ����ͻ�����
		for(i=0;i<Num;i++)
		{
			Dbus_TX_BUF[9+2*i] = Data[i]>>8;//���ݸ�
			Dbus_TX_BUF[10+2*i] = Data[i];//���ݵ�
		}
	 
		crctemp=dbus_CalcCrc(Dbus_TX_BUF,9+2*Num);
		
		Dbus_TX_BUF[8+2*Num+1] = crctemp>>8;//CRC��
		Dbus_TX_BUF[8+2*Num+2] = crctemp;//CRC��

		uart.printf_length(Dbus_TX_BUF,9+2*Num+2);

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

void AnalyzeDbus() //������������Ӧ����
{
	u16 crc;
	u8 c1,c2;
	
	//�ж�Ŀ���ַ
	if(((Dbus_Recive[3]<<8)|Dbus_Recive[4])==DbusLocalAddress)
	{
		//CRCУ��
		crc=dbus_CalcCrc(Dbus_Recive,(dbus_recivelength-2));  
		c1=crc>>8; //CRC���ֽ�
		c2=crc;    //CRC���ֽ�
		if(c1==Dbus_Recive[dbus_recivelength-2]&&c2==Dbus_Recive[dbus_recivelength-1])//У����ȷ
		{		
			//����
			if(Dbus_Recive[2]==1)//����֡
			{
				switch(Dbus_Recive[5])//������
				{
					case 0x01:RecFuc1();break;         //�������Ĵ���
					case 0x02:RecFuc2();break;         //д�����Ĵ���				      
					case 0x03:RecFuc3();break;	       //������Ĵ���
					case 0x04:RecFuc4();break;         //д����Ĵ���
					default:					break;  	
				}	
			}  
			else 
			if(Dbus_Recive[2]==2)//��Ӧ֡
			{
				response = 1;
			}
		} 
		
	}
}


/*�ӻ���Ӧ����*/

void send2_comm( void )
{
	
}

/*����01������*/ 
void RecFuc1()
 {

 } 
	  

/*����02д�����Ĵ���*/ 
void RecFuc2()
 {
	 u16 startadd;//��д��Ĵ�����ַ
	 u16 data;//��д������
	 
	 startadd = (Dbus_Recive[6]<<8)|Dbus_Recive[7];
	 data = (Dbus_Recive[8]<<8)|Dbus_Recive[9];
	 //��������
	 Dbus_Data[startadd] = data;
	 	 
	 //�ظ���Ӧ֡
	 responsedata((Dbus_Recive[0]<<8)|Dbus_Recive[1],Dbus_Recive[5],1);
 } 
	


/*����03�����뱨��*/ 
void RecFuc3()
 {

 }
 
/*����04д����Ĵ���*/ 
void RecFuc4()
 {
	 u16 startadd;//��д��Ĵ�����ַ
	 u16 len;//��д�����ݳ���
	 u16 i;
	 startadd = (Dbus_Recive[6]<<8)|Dbus_Recive[7];
	 len = Dbus_Recive[8];
	 for(i=0;i<len;i++)
	 {
		  Dbus_Data[startadd+i] = (Dbus_Recive[9+2*i]<<8)|Dbus_Recive[10+2*i];
	 }
	 
	 //�ظ���Ӧ֡
	 responsedata((Dbus_Recive[0]<<8)|Dbus_Recive[1],Dbus_Recive[5],1);
 }  


/// <summary>
/// ��Ӧ֡
/// </summary>
/// <param name="DstAdress">Ŀ���ַ</param>
/// <param name="func">������</param>
/// <param name="Data">���</param>
void responsedata(u16 DstAdress,u8 func,u8 resault)
{
	u16 crctemp=0;
	Dbus_TX_BUF[0] = DbusLocalAddress>>8;//������ַ��
	Dbus_TX_BUF[1] = DbusLocalAddress;//������ַ��
	Dbus_TX_BUF[2] = 2;//֡����
	Dbus_TX_BUF[3] = DstAdress>>8;//Ŀ���ַ��
	Dbus_TX_BUF[4] = DstAdress;//Ŀ���ַ��
	Dbus_TX_BUF[5] = func;//������
	Dbus_TX_BUF[6] = resault;//���
	 
	crctemp=dbus_CalcCrc(Dbus_TX_BUF,7);
	
	Dbus_TX_BUF[7] = crctemp>>8;//CRC��
	Dbus_TX_BUF[8] = crctemp;//CRC��
	
	uart.printf_length(Dbus_TX_BUF,9);
}

/*���󷵻�*/
void errorsend2(uint8_t func,uint8_t type)
{

}

 
/*************************************************
crc16У������㺯��,�����㷨��
1������crcУ��Ĵ����ĳ�ֵΪ0xffff;
2�������ĵĵ�һ���ֽ���crc�Ĵ����ĵ��ֽ���򣬽������crc�Ĵ���
3���ж�crc�ĵ�һλ�Ƿ�Ϊ1�������1,crc����1λ���0xa001������Ϊ0��������1λ��
4���ظ�����3��ֱ�����8��λ��
5���ظ�����2��3��4ֱ����������ֽ�
6�����ؼ�����
***********************************************/
uint16_t dbus_CalcCrc(char *chData,unsigned short uNo)
{
	uint16_t crc=0xffff;
	uint16_t i,j;
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


void clear_rxBuf2()
{
	u16 i=0;
	for(i=0;i<dbus_recivelength;i++)
			Dbus_Recive[i]=	0;
}
//src:ָ��
//dst:������Ϣ
//timeout:��ʱʱ��
int check(char* dst,u16 timeout,char* src,...)
{
	u16 num=0;
	char *resault;
	
	u16 i;
	va_list ap;
	va_start(ap,src);
	vsprintf((char*)Dbus_TX_BUF,src,ap);
	va_end(ap);
	i=strlen((const char*)Dbus_TX_BUF);//�˴η������ݵĳ���

    uart.printf_length(Dbus_TX_BUF,i);
	while(!resault&&num<(timeout/10.0))
	{
		resault=strstr(Dbus_Recive,dst);
		delay_ms(10);
		num++;
	}
	clear_rxBuf2();
	if(resault)
	{
		return 1;
	}
	else
		return 0;
}

//�Ƚ��ַ�����Ŀ���ַ
u16 ComperStr(u16 RegisterAdress,char* str)
{
	u16 len,i;
	len = strlen(str);//���ݳ���
	for(i=0;i<len;i++)
	{
		if(Dbus_Data[RegisterAdress+i/2]>>8 != str[i])
		{
			return 0;
		}
		if(i<(len-1))
		{
			if((Dbus_Data[RegisterAdress+i/2]&0xff) != str[i+1])
			{
				return 0;
			}
		}
		i++;
	}
	return 1;
}

//д�ַ�����Ŀ���ַ
u16 WriteStr(u16 DstAdress,u16 RegisterAdress,char* str)
{
	u16 len,i,len2=0;
	u16 data[100];
	len = strlen(str);//���ݳ���
	
	for(i=0;i<len;i++)
	{
		data[len2] = str[i]<<8;
		if(i<(len-1))
		{
			data[len2] |= str[i+1];
		}
		i++;
		len2++;
	}
	return Write_MultipleWord(DstAdress,RegisterAdress,len2,data);
}










