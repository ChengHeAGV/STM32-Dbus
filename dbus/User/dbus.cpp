#include "dbus.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"

//定义回调函数指针
typedef void (*callback_fun_type)(char*);
callback_fun_type SEND_CALLBACK;
typedef void (*callback_void_type)(void);
callback_void_type DELAY_CALLBACK;

//帧头
char DBUS_HEAD = '$'; 
//帧尾
char DBUS_END  = '!';
//正则表达式
char* DBUS_REGEX = "$%[^!]";
//帧ID
u16 FrameID=0;

/*********内部功能函数********************************************************************/
			
/*********CRC16校验*******************************************************************/
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





/*********发送函数******************************************************************/
void Send(char* buf,u8 len)
{
	//数据发送临时数组
	char *TX_BUF=(char *)malloc((len+2)*sizeof(char));//定义动态数组TX_BUF[len+2];
	unsigned int *TEMP=(unsigned int *)malloc((len)*sizeof(unsigned int));
	//消息头 
	TX_BUF[0] = DBUS_HEAD;

	//有效数据部分转成16进制ASCII码
	sscanf(buf,"%02X",TEMP);
	for(int i=0;i<len;i++)
	{
		TX_BUF[i+1]=TEMP[i];
	}
	//消息尾
	TX_BUF[len+1]=DBUS_END;
	//调用发送回调函数
	SEND_CALLBACK(TX_BUF);
	
	//释放动态开辟的空间
	free(TX_BUF);
	free(TEMP);
	/*为了防止野指针产生*/
  TX_BUF = NULL;
	TEMP   = NULL;
}




/*********将16进制字符串转数值******************************************************************/
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

/**********公共函数*******************************************************************/
Dbus::Dbus(u16 LocalAddress) 
{
	//本机地址
	this->LocalAddress = LocalAddress;
}


void Dbus::InPut(char c) 
{
	//将收到的数据追加到接收缓存
	DBUS_RECIVE_BUF[DBUS_RECIVE_LEN++] = c;
	//收到结束符触发解包函数
	if (DBUS_RECIVE_BUF[DBUS_RECIVE_LEN - 1] == DBUS_END)
	{
		OpenBox();
	}
}


/*********解包函数******************************************************************/
void Dbus::OpenBox()
{
	char* p;
	//向前移动的长度
	u8 MOVE_LEN=0;
	//单帧临时数组
	char temp[DBUS_MAX_LENGTH];
	//单帧转换后的数组
	char buf[DBUS_MAX_LENGTH];
	
	//初步排除无效数据(如果第一个开始符前有数据，则清除)
	p = strchr(DBUS_RECIVE_BUF,DBUS_HEAD);
	if (p)
	{
		//用MOVE_LEN记住移动前的数量
		MOVE_LEN = DBUS_RECIVE_LEN;
		//更新接收数量
		DBUS_RECIVE_LEN -= p - DBUS_RECIVE_BUF;
		//移动的数量
		MOVE_LEN=MOVE_LEN-DBUS_RECIVE_LEN;
		//移动有效数据
		for (int i = 0; i < DBUS_RECIVE_LEN; i++)
		{
			DBUS_RECIVE_BUF[i] = DBUS_RECIVE_BUF[i + p - DBUS_RECIVE_BUF];
		}
		//清除剩余数据
		for (int i = 0; i < MOVE_LEN; i++)
		{
			DBUS_RECIVE_BUF[i+DBUS_RECIVE_LEN] = 0;
		}
	}
	
	//循环搜索并解析有效数据
	while (DBUS_RECIVE_LEN > 0)
	{
		if (1 == sscanf(DBUS_RECIVE_BUF,DBUS_REGEX,temp))//
		{
			//由于上面条件的正则不判断是否有截止符，因此需满足以下条件(包含结束符)
			if(strchr(DBUS_RECIVE_BUF, DBUS_END))
			{
				//将分包的十六进制字符串temp转换为数值数组buf
				HexStrToDec(temp,buf);
				//执行解析函数
				Analyze(buf,strlen(temp));
				//清除第一个结束符之前的数据（已经使用过及之前的无效数据）
				p = strchr(DBUS_RECIVE_BUF, DBUS_END);
				if (p)
				{
					//用MOVE_LEN记住移动前的数量
					MOVE_LEN=DBUS_RECIVE_LEN;
					//更新接收数量
					DBUS_RECIVE_LEN -= p - DBUS_RECIVE_BUF + 1;
					//移动的数量
					MOVE_LEN=MOVE_LEN-DBUS_RECIVE_LEN;
					if (DBUS_RECIVE_LEN > 0)
					{
						//移动有效数据
						for (int i = 0; i < DBUS_RECIVE_LEN; i++)
						{
							DBUS_RECIVE_BUF[i] = DBUS_RECIVE_BUF[i + p - DBUS_RECIVE_BUF + 1];
						}
						//清除剩余数据
						for (int i = 0; i < MOVE_LEN; i++)
						{
							DBUS_RECIVE_BUF[i+DBUS_RECIVE_LEN] = 0;
						}
					}
					else
					{
						//避免出现负数
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



/*校验并解析数据帧
 *@Return          NONE 
*/
void Dbus::Analyze(char *buf ,u8 len)
{
	u16 CRC;
	u8 C1,C2;
	
	//判断目标地址是否为本机
	if(((buf[3]<<8)|buf[4]) == LocalAddress)
	{
		//CRC校验
		CRC=CRC_CALC(buf,(len-2));  
		C1=CRC>>8; //CRC高字节
		C2=CRC;    //CRC低字节
		if(C1==buf[len-2]&&C2==buf[len-1])//校验正确
		{		
			//解析
			if(buf[2]==1)//操作帧
			{
				switch(buf[5])//功能码
				{
					case 0x01:RecFuc1(buf,len);break; //读单个寄存器
					case 0x02:RecFuc2(buf,len);break; //写单个寄存器				      
					case 0x03:RecFuc3(buf,len);break; //读多个寄存器
					case 0x04:RecFuc4(buf,len);break; //写多个寄存器
					default:					break;  	
				}	
			}  
			else 
			if(buf[2]==2)//响应帧
			{
				//添加到响应帧缓冲池
				for(int i=0;i<DBUS_MAX_RESPONSE_BUF;i++)
				{
					//将响应帧编入空闲的响应帧缓冲池
					if(DBUS_RESPONSE_BUF[i][0]==0)
					{
						//缓冲池第1字节为帧长度
						DBUS_RESPONSE_BUF[i][0] = len;
						//将该响应帧加入缓冲池
						for(int j=0;i<len;j++)
							DBUS_RESPONSE_BUF[i][j+1] = buf[j];
					}
				}
			}
		} 
		
	}
}


/*发送中断函数定义
 *@Function        初始化发送中断函数
 *@callback_fun    目标函数
 *@Return          NONE 
*/
void Dbus::OutPut_interrupt(void (*callback_fun)(char*))
{
	SEND_CALLBACK = callback_fun;
}

/*延时中断函数定义
 *@callback_fun    目标函数
 *@Return          NONE 
*/
void Dbus::Delay_interrupt(void (*callback_delay)(void))
{
	DELAY_CALLBACK = callback_delay;
}

/*写单个寄存器
 *@Function        给单个寄存器写值
 *@TargetAdress    目标设备地址
 *@RegisterAddress 目标寄存器地址
 *@Data            待写入数值
 *@Return          1:写入成功，0:写入失败  
*/
u8 Dbus::Write_Register(u16 TargetAddress,u16 RegisterAddress,u16 Data)//写单个寄存器
{
	u16 CRC=0;	
	char TX_BUF[14];
	
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//帧ID高
	TX_BUF[1] = frameid;//帧ID低	
	TX_BUF[2] = LocalAddress>>8;//本机地址高
	TX_BUF[3] = LocalAddress;//本机地址低
	TX_BUF[4] = 1;//帧类型
	TX_BUF[5] = TargetAddress>>8;//目标地址高
	TX_BUF[6] = TargetAddress;//目标地址低
	TX_BUF[7] = 2;//功能码
	TX_BUF[8] = RegisterAddress>>8;//寄存器地址高
	TX_BUF[9] = RegisterAddress;//寄存器地址低
	TX_BUF[10] = Data>>8;//数据高
	TX_BUF[11] = Data;//数据低
 
	CRC=CRC_CALC(TX_BUF,12);
	
	TX_BUF[12] = CRC>>8;//CRC高
	TX_BUF[13] = CRC;//CRC低	
		
	for(j=0;j<DBUS_MAX_REPEAT_NUM; j++)
	{
		//发送数据
		Send(TX_BUF,14);
		//等待响应
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
			//延时1ms，防止实时系统调用时卡死
			DELAY_CALLBACK();
		}
	}
	return 0;
}




/*心跳函数
 *@Function        定时汇报设备在线
 *@TargetAdress    目标设备号     
 *@Return          1:汇报成功，0:汇报失败  
*/
void Dbus::Heart(u16 TargetAddress)//心跳函数
{
	//数据发送临时数组
	char TX_BUF[7];
	//存储CRC计算结果临时变量
	u16 CRC;
	TX_BUF[0] = LocalAddress >>8;//本机地址高
	TX_BUF[1] = LocalAddress;//本机地址低
	TX_BUF[2] = 0;//帧类型
	TX_BUF[3] = 0;//目标地址高
	TX_BUF[4] = 1;//目标地址低
	
	CRC=CRC_CALC(TX_BUF,5);
	
	TX_BUF[5] = CRC>>8;//CRC高
	TX_BUF[6] = CRC;   //CRC低
	
	Send(TX_BUF,7);
}













/*写多个寄存器
 *@TargetAdress    目标设备地址
 *@RegisterAddress 目标寄存器地址
 *@Num             待写入寄存器个数
 *@Data            待写入数值指针
 *@Return          1:写入成功，0:写入失败  
*/
u8 Write_MultipleWord(u16 TargetAdress,u16 RegisterAddress,u8 Num,u16* Data)//写单个寄存器
{
	u16 CRC=0;
	char TX_BUF[14];
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//帧ID高
	TX_BUF[1] = frameid;//帧ID低		
	TX_BUF[2] = DbusLocalAddress>>8;//本机地址高
	TX_BUF[3] = DbusLocalAddress;//本机地址低
	TX_BUF[4] = 1;//帧类型
	TX_BUF[5] = TargetAdress>>8;//目标地址高
	TX_BUF[6] = TargetAdress;//目标地址低
	TX_BUF[7] = 4;//功能码
	TX_BUF[8] = RegisterAddress>>8;//寄存器地址高
	TX_BUF[9] = RegisterAddress;//寄存器地址低
	TX_BUF[10] = Num;//寄存器数量

	//循环写入数据到发送缓冲区
	for(i=0;i<Num;i++)
	{
		TX_BUF[11+2*i] = Data[i]>>8;//数据高
		TX_BUF[12+2*i] = Data[i];//数据低
	}

	CRC=CRC_CALC(TX_BUF,11+2*Num);

	TX_BUF[8+2*Num+1] = CRC>>8;//CRC高
	TX_BUF[8+2*Num+2] = CRC;//CRC低

	uart.printf_length(TX_BUF,9+2*Num+2);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	for(j=0;j<repeatNum; j++)
	{


		//等待响应
		timeout=0;
		while(response==0&&(timeout<responseTime/10))//
		{
			timeout++;
			delay_ms(10);
		}
		if(timeout<(responseTime/10))//正常响应，结束
		{
			j = repeatNum;
			response = 0;		
			return 1;
		}
		
		//已重发次数加一
		num++;
		
		if(timeout<(responseTime/10))//超时
		{
			if(num==repeatNum)//重发次数到达上限，发送失败，返回0
			{
				return 0;
			}
		}
	}
	return 1;
}






/*读单个寄存器*/ 
void RecFuc1(char *buf ,u8 len)
 {

 } 
	  

/*写单个寄存器*/ 
void RecFuc2(char *buf ,u8 len)
 {
//	 u16 startadd;//待写入寄存器地址
//	 u16 data;//待写入数据
//	 
//	 startadd = (Dbus_Recive[6]<<8)|Dbus_Recive[7];
//	 data = (Dbus_Recive[8]<<8)|Dbus_Recive[9];
//	 //更新数据
//	 Dbus_Data[startadd] = data;
//	 	 
//	 //回复响应帧
//	 responsedata((Dbus_Recive[0]<<8)|Dbus_Recive[1],Dbus_Recive[5],1);
 } 
	


/*读多个寄存器*/ 
void RecFuc3(char *buf ,u8 len)
 {

 }
 
/*写多个寄存器*/ 
void RecFuc4(char *buf ,u8 len)
 {
//	 u16 startadd;//待写入寄存器地址
//	 u16 len;//待写入数据长度
//	 u16 i;
//	 startadd = (Dbus_Recive[6]<<8)|Dbus_Recive[7];
//	 len = Dbus_Recive[8];
//	 for(i=0;i<len;i++)
//	 {
//		  Dbus_Data[startadd+i] = (Dbus_Recive[9+2*i]<<8)|Dbus_Recive[10+2*i];
//	 }
//	 
//	 //回复响应帧
//	 responsedata((Dbus_Recive[0]<<8)|Dbus_Recive[1],Dbus_Recive[5],1);
 }  


///// <summary>
///// 响应帧
///// </summary>
///// <param name="DstAdress">目标地址</param>
///// <param name="func">功能码</param>
///// <param name="Data">结果</param>
//void responsedata(u16 DstAdress,u8 func,u8 resault)
//{
////	u16 crctemp=0;
////	Dbus_TX_BUF[0] = DbusLocalAddress>>8;//本机地址高
////	Dbus_TX_BUF[1] = DbusLocalAddress;//本机地址低
////	Dbus_TX_BUF[2] = 2;//帧类型
////	Dbus_TX_BUF[3] = DstAdress>>8;//目标地址高
////	Dbus_TX_BUF[4] = DstAdress;//目标地址低
////	Dbus_TX_BUF[5] = func;//功能码
////	Dbus_TX_BUF[6] = resault;//结果
////	 
////	crctemp=dbus_CalcCrc(Dbus_TX_BUF,7);
////	
////	Dbus_TX_BUF[7] = crctemp>>8;//CRC高
////	Dbus_TX_BUF[8] = crctemp;//CRC低
////	
////	uart.printf_length(Dbus_TX_BUF,9);
//}


////src:指令
////dst:返回信息
////timeout:超时时间
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
////	i=strlen((const char*)Dbus_TX_BUF);//此次发送数据的长度

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

////比较字符串和目标地址
//u16 ComperStr(u16 RegisterAdress,char* str)
//{
//	u16 len,i;
//	len = strlen(str);//数据长度
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

////写字符串到目标地址
//u16 WriteStr(u16 DstAdress,u16 RegisterAdress,char* str)
//{
//	u16 len,i,len2=0;
//	u16 data[100];
//	len = strlen(str);//数据长度
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










