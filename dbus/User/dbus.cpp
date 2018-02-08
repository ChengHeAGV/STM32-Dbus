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
	if(((buf[5]<<8)|buf[6]) == LocalAddress)
	{
		//CRCУ��
		CRC=CRC_CALC(buf,(len-2));  
		C1=CRC>>8; //CRC���ֽ�
		C2=CRC;    //CRC���ֽ�
		if(C1==buf[len-2]&&C2==buf[len-1])//У����ȷ
		{		
			//����
			if(buf[4]==1)//����֡
			{
				switch(buf[7])//������
				{
					case 0x01:Response_Read_Register(buf);break; //�������Ĵ���
					case 0x02:Response_Write_Register(buf);break; //д�����Ĵ���				      
					case 0x03:Response_Read_Multiple_Registers(buf);break; //������Ĵ���
					case 0x04:Response_Write_Multiple_Registers(buf);break; //д����Ĵ���
					default:break;  	
				}	
			}  
			else 
			if(buf[4]==2)//��Ӧ֡
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
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//֡ID��
	TX_BUF[1] = frameid;//֡ID��		
	TX_BUF[2] = LocalAddress >>8;//������ַ��
	TX_BUF[3] = LocalAddress;//������ַ��
	TX_BUF[4] = 0;//֡����
	TX_BUF[5] = 0;//Ŀ���ַ��
	TX_BUF[6] = 1;//Ŀ���ַ��
	
	CRC=CRC_CALC(TX_BUF,7);
	
	TX_BUF[7] = CRC>>8;//CRC��
	TX_BUF[8] = CRC;   //CRC��
	
	Send(TX_BUF,9);
}

//�������Ĵ���
ReturnMsg Dbus::Read_Register(u16 TargetAddress,u16 RegisterAddress)
{
	u16 CRC=0;	
	char TX_BUF[12];
	ReturnMsg msg;
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//֡ID��
	TX_BUF[1] = frameid;//֡ID��	
	TX_BUF[2] = LocalAddress>>8;//������ַ��
	TX_BUF[3] = LocalAddress;//������ַ��
	TX_BUF[4] = 1;//֡����
	TX_BUF[5] = TargetAddress>>8;//Ŀ���ַ��
	TX_BUF[6] = TargetAddress;//Ŀ���ַ��
	TX_BUF[7] = 1;//������
	TX_BUF[8] = RegisterAddress>>8;//�Ĵ�����ַ��
	TX_BUF[9] = RegisterAddress;//�Ĵ�����ַ��

	CRC=CRC_CALC(TX_BUF,10);
	
	TX_BUF[10] = CRC>>8;//CRC��
	TX_BUF[11] = CRC;//CRC��	
		
	for(int j=0;j<DBUS_MAX_REPEAT_NUM; j++)
	{
		//��������
		Send(TX_BUF,12);
		//�ȴ���Ӧ
		for(int k=0;k<DBUS_TIMEOUT;k++)
		{
			for(int i=0;i<DBUS_MAX_RESPONSE_BUF ;i++)
			{
				if(DBUS_RESPONSE_BUF[i][0]!=0)
				{
					if(((DBUS_RESPONSE_BUF[i][1]<<8)|DBUS_RESPONSE_BUF[i][2]) == frameid)
					{
						msg.resault = 1;
						msg.Data = DBUS_RESPONSE_BUF[i][10]<<8|DBUS_RESPONSE_BUF[i][11];
						DBUS_RESPONSE_BUF[i][0]=0;
						return msg;
					}
				}
			}
			//��ʱ1ms����ֹʵʱϵͳ����ʱ����
			DELAY_CALLBACK();
		}
	}
	msg.resault = 0;
	return msg;
}
//������Ĵ���
ReturnMsg Dbus::Read_Multiple_Registers(u16 TargetAddress,u16 RegisterAddress,u8 Num)
{
	u16 CRC=0;	
	char TX_BUF[13];
	ReturnMsg msg;
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//֡ID��
	TX_BUF[1] = frameid;//֡ID��	
	TX_BUF[2] = LocalAddress>>8;//������ַ��
	TX_BUF[3] = LocalAddress;//������ַ��
	TX_BUF[4] = 1;//֡����
	TX_BUF[5] = TargetAddress>>8;//Ŀ���ַ��
	TX_BUF[6] = TargetAddress;//Ŀ���ַ��
	TX_BUF[7] = 3;//������
	TX_BUF[8] = RegisterAddress>>8;//�Ĵ�����ַ��
	TX_BUF[9] = RegisterAddress;//�Ĵ�����ַ��
	TX_BUF[10] = Num;//����ȡ�Ĵ�������

	CRC=CRC_CALC(TX_BUF,11);
	
	TX_BUF[11] = CRC>>8;//CRC��
	TX_BUF[12] = CRC;//CRC��	
		
	for(int j=0;j<DBUS_MAX_REPEAT_NUM; j++)
	{
		//��������
		Send(TX_BUF,13);
		//�ȴ���Ӧ
		for(int k=0;k<DBUS_TIMEOUT;k++)
		{
			for(int i=0;i<DBUS_MAX_RESPONSE_BUF ;i++)
			{
				if(DBUS_RESPONSE_BUF[i][0]!=0)
				{
					if(((DBUS_RESPONSE_BUF[i][1]<<8)|DBUS_RESPONSE_BUF[i][2]) == frameid)
					{
						msg.resault = 1;
						for(int t=0;t<Num;t++)
						{
							msg.DataBuf[t] = DBUS_RESPONSE_BUF[i][11+2*t]<<8|DBUS_RESPONSE_BUF[i][12+2*t];
						}
						DBUS_RESPONSE_BUF[i][0]=0;
						return msg;
					}
				}
			}
			//��ʱ1ms����ֹʵʱϵͳ����ʱ����
			DELAY_CALLBACK();
		}
	}
	msg.resault = 0;
	return msg;
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
		
	for(int j=0;j<DBUS_MAX_REPEAT_NUM; j++)
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





/*д����Ĵ���
 *@TargetAdress    Ŀ���豸��ַ
 *@RegisterAddress Ŀ��Ĵ�����ַ
 *@Num             ��д��Ĵ�������
 *@Data            ��д����ֵָ��
 *@Return          1:д��ɹ���0:д��ʧ��  
*/
u8 Dbus:: Write_Multiple_Registers(u16 TargetAdress,u16 RegisterAddress,u8 Num,u16* Data)//д�����Ĵ���
{
	u16 CRC=0;
	char *TX_BUF=(char *)malloc((11+2*Num+2)*sizeof(char));//���嶯̬����TX_BUF[11+2*Num+2];
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//֡ID��
	TX_BUF[1] = frameid;//֡ID��		
	TX_BUF[2] = LocalAddress>>8;//������ַ��
	TX_BUF[3] = LocalAddress;//������ַ��
	TX_BUF[4] = 1;//֡����
	TX_BUF[5] = TargetAdress>>8;//Ŀ���ַ��
	TX_BUF[6] = TargetAdress;//Ŀ���ַ��
	TX_BUF[7] = 4;//������
	TX_BUF[8] = RegisterAddress>>8;//�Ĵ�����ַ��
	TX_BUF[9] = RegisterAddress;//�Ĵ�����ַ��
	TX_BUF[10] = Num;//�Ĵ�������

	//ѭ��д�����ݵ����ͻ�����
	for(int i=0;i<Num;i++)
	{
		TX_BUF[11+2*i] = Data[i]>>8;//���ݸ�
		TX_BUF[12+2*i] = Data[i];//���ݵ�
	}

	CRC=CRC_CALC(TX_BUF,11+2*Num);

	TX_BUF[11+2*Num] = CRC>>8;//CRC��
	TX_BUF[11+2*Num+1] = CRC;//CRC��
	
	for(int j=0;j<DBUS_MAX_REPEAT_NUM; j++)
	{
		//��������
		Send(TX_BUF,11+2*Num+2);
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
						
						//�ͷŶ�̬���ٵĿռ�
						free(TX_BUF);
						/*Ϊ�˷�ֹҰָ�����*/
						TX_BUF = NULL;
						return 1;
					}
				}
			}
			//��ʱ1ms����ֹʵʱϵͳ����ʱ����
			DELAY_CALLBACK();
		}
	}
	
	//�ͷŶ�̬���ٵĿռ�
	free(TX_BUF);
	/*Ϊ�˷�ֹҰָ�����*/
  TX_BUF = NULL;
	return 0;
}



/*��Ӧ�������Ĵ���*/ 
void Dbus::Response_Read_Register(char *buf)
{
	u16 CRC=0;	
	char TX_BUF[14];
	//����ȡ�Ĵ�����ַ
	u16 regAdd = buf[8]<<8|buf[9];
	
	TX_BUF[0] = buf[0];//֡ID��
	TX_BUF[1] = buf[1];//֡ID��	
	TX_BUF[2] = LocalAddress>>8;//������ַ��
	TX_BUF[3] = LocalAddress;//������ַ��
	TX_BUF[4] = 2;//֡����
	TX_BUF[5] = buf[5];//Ŀ���ַ��
	TX_BUF[6] = buf[6];//Ŀ���ַ��
	TX_BUF[7] = 1;//������
	TX_BUF[8] = buf[8];//�Ĵ�����ַ��
	TX_BUF[9] = buf[9];//�Ĵ�����ַ��
	
	//��������ַ�������ƣ���������Ϊ0xFFFF
	if(regAdd>DBUS_REGISTER_LENGTH)
	{
		TX_BUF[10] = 0xFF;//���ݸ�
		TX_BUF[11] = 0xFF;//���ݵ�
	}
	else
	{
		TX_BUF[10] = Dbus_Register[regAdd]>>8;//���ݸ�
		TX_BUF[11] = Dbus_Register[regAdd];//���ݵ�
	}
	CRC=CRC_CALC(TX_BUF,12);
	TX_BUF[12] = CRC>>8;//CRC��
	TX_BUF[13] = CRC;//CRC��	
	
	//��������
	Send(TX_BUF,14);		
}   

/*��Ӧд�����Ĵ���*/ 
void Dbus:: Response_Write_Register(char *buf)
 {
	u16 CRC=0;	
	char TX_BUF[11];
	//��д��Ĵ�����ַ
	u16 regAdd = buf[8]<<8|buf[9];
	//��д������
	u16 data = (buf[10]<<8)|buf[11];
	 	 
	//�ظ���Ӧ֡
	TX_BUF[0] = buf[0];//֡ID��
	TX_BUF[1] = buf[1];//֡ID��	
	TX_BUF[2] = LocalAddress>>8;//������ַ��
	TX_BUF[3] = LocalAddress;//������ַ��
	TX_BUF[4] = 2;//֡����
	TX_BUF[5] = buf[5];//Ŀ���ַ��
	TX_BUF[6] = buf[6];//Ŀ���ַ��
	TX_BUF[7] = 2;//������
	
	if(regAdd>DBUS_REGISTER_LENGTH)
	{
		TX_BUF[8] = 0;//���	
	}
	else
	{
		TX_BUF[8] = 1;//���	
		//��������
		Dbus::Dbus_Register[regAdd] = data;
	}
		
	CRC=CRC_CALC(TX_BUF,9);
	
	TX_BUF[9] = CRC>>8;//CRC��
	TX_BUF[10] = CRC;//CRC��	
	
	//��������
	Send(TX_BUF,11);	
} 
 
/*��Ӧ������Ĵ���*/ 
void Dbus:: Response_Read_Multiple_Registers(char *buf)
{
	u16 CRC=0;	
	u8 Num = buf[10];
	char *TX_BUF=(char *)malloc((11+2*Num+2)*sizeof(char));//���嶯̬����TX_BUF[11+2*Num+2];
	//����ȡ�Ĵ�����ʼ��ַ
	u16 regStartAdd = buf[8]<<8|buf[9];
	
	TX_BUF[0] = buf[0];//֡ID��
	TX_BUF[1] = buf[1];//֡ID��	
	TX_BUF[2] = LocalAddress>>8;//������ַ��
	TX_BUF[3] = LocalAddress;//������ַ��
	TX_BUF[4] = 2;//֡����
	TX_BUF[5] = buf[5];//Ŀ���ַ��
	TX_BUF[6] = buf[6];//Ŀ���ַ��
	TX_BUF[7] = 3;//������
	TX_BUF[8] = buf[8];//�Ĵ�����ʼ��ַ��
	TX_BUF[9] = buf[9];//�Ĵ�����ʼ��ַ��
	TX_BUF[10] = buf[10];//����
	
	for(int i=0;i<Num;i++)
	{
		//��������ַ�������ƣ���������Ϊ0xFFFF
		if((regStartAdd+i)>DBUS_REGISTER_LENGTH)
		{
			TX_BUF[11+i*2] = 0xFF;//���ݸ�
			TX_BUF[12+i*2] = 0xFF;//���ݵ�
		}
		else
		{
			TX_BUF[11+i*2] = Dbus::Dbus_Register[regStartAdd+i]>>8;//���ݸ�
			TX_BUF[12+i*2] = Dbus::Dbus_Register[regStartAdd+i];//���ݵ�
		}
	}

	CRC=CRC_CALC(TX_BUF,11+Num*2);
	
	TX_BUF[11+Num*2] = CRC>>8;//CRC��
	TX_BUF[11+Num*2+1] = CRC;//CRC��	
	
	//��������
	Send(TX_BUF,11+Num*2+2);
	
	//�ͷŶ�̬���ٵĿռ�
	free(TX_BUF);
	/*Ϊ�˷�ֹҰָ�����*/
  TX_BUF = NULL;	
}
 
/*��Ӧд����Ĵ���*/ 
void Dbus:: Response_Write_Multiple_Registers(char *buf)
 {
	u16 CRC=0;	
	char TX_BUF[11];
	//��д��Ĵ�����ʼ��ַ
	u16 regStartAdd = buf[8]<<8|buf[9];
	u8 Num = buf[10];
	//�ظ���Ӧ֡
	TX_BUF[0] = buf[0];//֡ID��
	TX_BUF[1] = buf[1];//֡ID��	
	TX_BUF[2] = LocalAddress>>8;//������ַ��
	TX_BUF[3] = LocalAddress;//������ַ��
	TX_BUF[4] = 2;//֡����
	TX_BUF[5] = buf[5];//Ŀ���ַ��
	TX_BUF[6] = buf[6];//Ŀ���ַ��
	TX_BUF[7] = 4;//������
	TX_BUF[8] = 1;//���
	for(int i=0;i<Num;i++) 
	{
		if((regStartAdd+i)>DBUS_REGISTER_LENGTH)
		{
			TX_BUF[8] = 0;//���	
		}
		else
		{
			TX_BUF[8] = 1;//���	
			//��������
			Dbus::Dbus_Register[regStartAdd+i] = buf[11]<<8|buf[12];
		}
	}

	CRC=CRC_CALC(TX_BUF,9);
	
	TX_BUF[9] = CRC>>8;//CRC��
	TX_BUF[10] = CRC;//CRC��	
	
	//��������
	Send(TX_BUF,11);		 
 }  




