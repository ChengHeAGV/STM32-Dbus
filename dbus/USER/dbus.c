#include "dbus.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"

//本机地址
u16 LocalAddress;
//寄存器
u16 Dbus_Register[DBUS_REGISTER_LENGTH];
//数据接收缓冲池
char DBUS_RECIVE_BUF[DBUS_MAX_RECIVE_BUF];
//响应消息队列缓冲池长度
char DBUS_RESPONSE_BUF[DBUS_MAX_RESPONSE_BUF][DBUS_MAX_LENGTH];

//接收双缓冲
char DBUS_RECIVE_DOUBLE_BUF[DBUS_MAX_RECIVE_BUF];
//接收双双缓冲长度
u16 DBUS_RECIVE_DOUBLE_LEN;


//接收缓冲池中数据长度（当前长度）
u16 DBUS_RECIVE_LEN;

//定义回调函数指针
typedef void (*callback_fun_type)(char*,u16);
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
void Send(u8* buf,u16 len)
{
	//数据发送临时数组
	u8 TX_BUF[DBUS_MAX_LENGTH];
	u8 hight=0;
	u8 low=0;
	
	//消息头 
	TX_BUF[0] = DBUS_HEAD;

	//有效数据部分转成16进制ASCII码
	for(u16 i=0;i<len;i++)
	{
		hight = buf[i]/0x10;//取高位
		low   = buf[i]%0x10;//取低位
		
		if((hight>=0)&&(hight<=9))  
		{  
			hight += 0x30;  
		}  
		else if((hight>=10)&&(hight<=15))//Capital  
		{  
			hight += 0x37;  
		}  
		
		if((low>=0)&&(low<=9))  
		{  
			low += 0x30;  
		}  
		else if((low>=10)&&(low<=15))//Capital  
		{  
			low += 0x37;  
		} 
		
		TX_BUF[2*i+1] = hight;
		TX_BUF[2*i+2] = low;
	}
	
	//消息尾
	TX_BUF[2*len+1]=DBUS_END;
	//调用发送回调函数
	SEND_CALLBACK(TX_BUF,2*len+2);
}

/*********将16进制字符串转数值******************************************************************/
void HexStrToDec(char* str,u8* dec)
{
	u16 l;
  u16 hight=0;
	u16 low=0  ;
  l=strlen(str);
    
	for(int i=0;i<l ;i++)
	{   
        hight = str[i+0];//取高位
	    low   = str[i+1];//取低位
		
		if((hight>=0x30)&&(hight<=0x39))  
		{  
			hight -= 0x30;  
		} 
        else
        if((hight>=0x41)&&(hight<=0x46))//Capital  
		{  
			hight -= 0x37;  
		}  
        
        if((low>=0x30)&&(low<=0x39))  
		{  
			low -= 0x30;  
		} 
        else
        if((low>=0x41)&&(low<=0x46))//Capital  
		{  
			low -= 0x37;  
		}  
        
		dec[i/2] = hight*0x10+low;
		i++;
	}
}


/**********公共函数*******************************************************************/
void Dbus_Init(u16 Address) 
{
	//本机地址
	LocalAddress = Address;
}


void InPut(char c) 
{
	//将收到的数据追加到接收缓存
    //if(DBUS_RECIVE_LEN<DBUS_MAX_RECIVE_BUF)
        DBUS_RECIVE_BUF[DBUS_RECIVE_LEN++] = c;
//    else
//    {
//        //超过最大长度限制
//        DBUS_RECIVE_BUF[DBUS_RECIVE_LEN-1] = DBUS_END;
//    }
}


/*********解包函数******************************************************************/
void OpenBox()
{
	//单帧临时数组
	char temp[DBUS_MAX_LENGTH];
	//单帧转换后的数组
	char buf[DBUS_MAX_LENGTH];
	    
    //复制缓冲区数据到双缓冲
    memcpy(DBUS_RECIVE_DOUBLE_BUF,DBUS_RECIVE_BUF,DBUS_RECIVE_LEN);
    //更新双缓冲长度
    DBUS_RECIVE_DOUBLE_LEN = DBUS_RECIVE_LEN;

    //复位接收缓冲区
    memset(DBUS_RECIVE_BUF,0,DBUS_MAX_RECIVE_BUF);
    //清空接收缓冲长度
    DBUS_RECIVE_LEN = 0;
    
    //开始标志
    u16 Start = 0; 
    //结束标志
    u16 Stop = 0;
    //搜索结果
    u8 reault = 0;
    
    //循环搜索并解析有效数据
	while (DBUS_RECIVE_DOUBLE_LEN>(Stop+1))
	{
        reault = 0;
		for(u16 i = Stop;i<DBUS_RECIVE_DOUBLE_LEN;i++)
		{
			//查找开始符
			if(DBUS_RECIVE_DOUBLE_BUF[i] == DBUS_HEAD)
			{
				Start = i;
			}
			//查找结束符
			if((Start>=Stop)&&DBUS_RECIVE_DOUBLE_BUF[i] == DBUS_END)
			{
				Stop = i;
                reault = 1;
                break;
			}
		}
        if(reault == 1)//找到了有效的数据
        {
            //如果头尾中间没有数据，不处理
            if((Stop-Start)==1)
            {
                continue;
            }
            else
            {
                //将获取到的数据拷贝到临时数组
                memcpy(temp,&DBUS_RECIVE_DOUBLE_BUF[Start+1],Stop-Start+1-2);
                //将分包的十六进制字符串temp转换为数值数组buf
                HexStrToDec(temp,buf);
                //执行解析函数
                Analyze(buf,(Stop-Start+1-2)/2);//指针之差是字符串长度，应该除2
            }
            
        }
		else//没有找到有效数据
        {
            //退出循环
            break;
        }
        //延时10ms，防止实时系统调用时卡死
		DELAY_CALLBACK();
	}
    //复位接收缓冲区
   memset(DBUS_RECIVE_DOUBLE_BUF,0,DBUS_MAX_RECIVE_BUF);
}



/*校验并解析数据帧
 *@Return          NONE 
*/
void Analyze(char *buf ,u16 len)
{
	u16 CRC;
	u8 C1,C2;
	
	//判断目标地址是否为本机
	if(((buf[5]<<8)|buf[6]) == LocalAddress)
	{
		//CRC校验
		CRC=CRC_CALC(buf,(len-2));  
		C1=CRC>>8; //CRC高字节
		C2=CRC;    //CRC低字节
		if(C1==buf[len-2]&&C2==buf[len-1])//校验正确
		{		
			//解析
			if(buf[4]==1)//操作帧
			{
				switch(buf[7])//功能码
				{
					case 0x01:Response_Read_Register(buf);break; //读单个寄存器
					case 0x02:Response_Write_Register(buf);break; //写单个寄存器				      
					case 0x03:Response_Read_Multiple_Registers(buf);break; //读多个寄存器
					case 0x04:Response_Write_Multiple_Registers(buf);break; //写多个寄存器
					default:break;  	
				}	
			}  
			else 
			if(buf[4]==2)//响应帧
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
						for(int j=0;j<len;j++)
							DBUS_RESPONSE_BUF[i][j+1] = buf[j];
					}
                    len=len;
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
void OutPut_interrupt(void (*callback_fun)(char*,u16))
{
	SEND_CALLBACK = callback_fun;
}

/*延时中断函数定义
 *@callback_fun    目标函数
 *@Return          NONE 
*/
void Delay_interrupt(void (*callback_delay)(void))
{
	DELAY_CALLBACK = callback_delay;
}
/*心跳函数
 *@Function        定时汇报设备在线
 *@TargetAdress    目标设备号     
 *@Return          1:汇报成功，0:汇报失败  
*/
void Heart(u16 TargetAddress)//心跳函数
{
	//数据发送临时数组
	char TX_BUF[11];
	//存储CRC计算结果临时变量
	u16 CRC;
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//帧ID高
	TX_BUF[1] = frameid;//帧ID低		
	TX_BUF[2] = LocalAddress >>8;//本机地址高
	TX_BUF[3] = LocalAddress;//本机地址低
	TX_BUF[4] = 0;//帧类型
	TX_BUF[5] = TargetAddress>>8;//目标地址高
	TX_BUF[6] = TargetAddress;//目标地址低
	
	CRC=CRC_CALC(TX_BUF,7);
	
	TX_BUF[7] = CRC>>8;//CRC高
	TX_BUF[8] = CRC;   //CRC低

	Send(TX_BUF,9);
}

//读单个寄存器
struct ReturnMsg Read_Register(u16 TargetAddress,u16 RegisterAddress)
{
	u16 CRC=0;	
	char TX_BUF[12];
	struct ReturnMsg msg;
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//帧ID高
	TX_BUF[1] = frameid;//帧ID低	
	TX_BUF[2] = LocalAddress>>8;//本机地址高
	TX_BUF[3] = LocalAddress;//本机地址低
	TX_BUF[4] = 1;//帧类型
	TX_BUF[5] = TargetAddress>>8;//目标地址高
	TX_BUF[6] = TargetAddress;//目标地址低
	TX_BUF[7] = 1;//功能码
	TX_BUF[8] = RegisterAddress>>8;//寄存器地址高
	TX_BUF[9] = RegisterAddress;//寄存器地址低

	CRC=CRC_CALC(TX_BUF,10);
	
	TX_BUF[10] = CRC>>8;//CRC高
	TX_BUF[11] = CRC;//CRC低	
		
	for(int j=0;j<DBUS_MAX_REPEAT_NUM; j++)
	{
		//发送数据
		Send(TX_BUF,12);
		//等待响应
		for(int k=0;k<DBUS_TIMEOUT;k++)
		{
			for(int i=0;i<DBUS_MAX_RESPONSE_BUF ;i++)
			{
				if(DBUS_RESPONSE_BUF[i][0]!=0)
				{
					if(((DBUS_RESPONSE_BUF[i][1]<<8)|DBUS_RESPONSE_BUF[i][2]) == frameid)
					{
						msg.resault = 1;
						msg.Data = DBUS_RESPONSE_BUF[i][11]<<8|DBUS_RESPONSE_BUF[i][12];
						DBUS_RESPONSE_BUF[i][0]=0;
						return msg;
					}
				}
			}
			//延时10ms，防止实时系统调用时卡死
			DELAY_CALLBACK();
		}
	}
	msg.resault = 0;
	return msg;
}
//读多个寄存器
struct ReturnMsg Read_Multiple_Registers(u16 TargetAddress,u16 RegisterAddress,u16 Num)
{
	u16 CRC=0;	
	char TX_BUF[13];
	struct ReturnMsg msg;
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//帧ID高
	TX_BUF[1] = frameid;//帧ID低	
	TX_BUF[2] = LocalAddress>>8;//本机地址高
	TX_BUF[3] = LocalAddress;//本机地址低
	TX_BUF[4] = 1;//帧类型
	TX_BUF[5] = TargetAddress>>8;//目标地址高
	TX_BUF[6] = TargetAddress;//目标地址低
	TX_BUF[7] = 3;//功能码
	TX_BUF[8] = RegisterAddress>>8;//寄存器地址高
	TX_BUF[9] = RegisterAddress;//寄存器地址低
	TX_BUF[10] = Num;//待读取寄存器数量

	CRC=CRC_CALC(TX_BUF,11);
	
	TX_BUF[11] = CRC>>8;//CRC高
	TX_BUF[12] = CRC;//CRC低	
		
	for(int j=0;j<DBUS_MAX_REPEAT_NUM; j++)
	{
		//发送数据
		Send(TX_BUF,13);
		//等待响应
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
							msg.DataBuf[t] = DBUS_RESPONSE_BUF[i][12+2*t]<<8|DBUS_RESPONSE_BUF[i][13+2*t];
						}
						DBUS_RESPONSE_BUF[i][0]=0;
						return msg;
					}
				}
			}
			//延时1ms，防止实时系统调用时卡死
			DELAY_CALLBACK();
		}
	}
	msg.resault = 0;
	return msg;
}
/*写单个寄存器
 *@Function        给单个寄存器写值
 *@TargetAdress    目标设备地址
 *@RegisterAddress 目标寄存器地址
 *@Data            待写入数值
 *@Return          1:写入成功，0:写入失败  
*/
u8 Write_Register(u16 TargetAddress,u16 RegisterAddress,u16 Data)//写单个寄存器
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
		
	for(int j=0;j<DBUS_MAX_REPEAT_NUM; j++)
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
			//延时10ms，防止实时系统调用时卡死
			DELAY_CALLBACK();
		}
	}
	return 0;
}





/*写多个寄存器
 *@TargetAdress    目标设备地址
 *@RegisterAddress 目标寄存器地址
 *@Num             待写入寄存器个数
 *@Data            待写入数值指针
 *@Return          1:写入成功，0:写入失败  
*/
u8 Write_Multiple_Registers(u16 TargetAdress,u16 RegisterAddress,u16 Num,u16* Data)//写单个寄存器
{
	u16 CRC=0;
	char TX_BUF[DBUS_MAX_LENGTH];
	FrameID++;
	u16 frameid = FrameID;
	
	TX_BUF[0] = frameid>>8;//帧ID高
	TX_BUF[1] = frameid;//帧ID低		
	TX_BUF[2] = LocalAddress>>8;//本机地址高
	TX_BUF[3] = LocalAddress;//本机地址低
	TX_BUF[4] = 1;//帧类型
	TX_BUF[5] = TargetAdress>>8;//目标地址高
	TX_BUF[6] = TargetAdress;//目标地址低
	TX_BUF[7] = 4;//功能码
	TX_BUF[8] = RegisterAddress>>8;//寄存器地址高
	TX_BUF[9] = RegisterAddress;//寄存器地址低
	TX_BUF[10] = Num;//寄存器数量

	//循环写入数据到发送缓冲区
	for(int i=0;i<Num;i++)
	{
		TX_BUF[11+2*i] = Data[i]>>8;//数据高
		TX_BUF[12+2*i] = Data[i];//数据低
	}

	CRC=CRC_CALC(TX_BUF,11+2*Num);

	TX_BUF[11+2*Num] = CRC>>8;//CRC高
	TX_BUF[11+2*Num+1] = CRC;//CRC低
	
	for(int j=0;j<DBUS_MAX_REPEAT_NUM; j++)
	{
		//发送数据
		Send(TX_BUF,11+2*Num+2);
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



/*响应读单个寄存器*/ 
void Response_Read_Register(char *buf)
{
	u16 CRC=0;	
	char TX_BUF[14];
	//待读取寄存器地址
	u16 regAdd = buf[8]<<8|buf[9];
	
	TX_BUF[0] = buf[0];//帧ID高
	TX_BUF[1] = buf[1];//帧ID低	
	TX_BUF[2] = LocalAddress>>8;//本机地址高
	TX_BUF[3] = LocalAddress;//本机地址低
	TX_BUF[4] = 2;//帧类型
	TX_BUF[5] = buf[5];//目标地址高
	TX_BUF[6] = buf[6];//目标地址低
	TX_BUF[7] = 1;//功能码
	TX_BUF[8] = buf[8];//寄存器地址高
	TX_BUF[9] = buf[9];//寄存器地址低
	
	//如果请求地址超出限制，返回数据为0xFFFF
	if(regAdd>DBUS_REGISTER_LENGTH)
	{
		TX_BUF[10] = 0xFF;//数据高
		TX_BUF[11] = 0xFF;//数据低
	}
	else
	{
		TX_BUF[10] = Dbus_Register[regAdd]>>8;//数据高
		TX_BUF[11] = Dbus_Register[regAdd];//数据低
	}
	CRC=CRC_CALC(TX_BUF,12);
	TX_BUF[12] = CRC>>8;//CRC高
	TX_BUF[13] = CRC;//CRC低	
	
	//发送数据
	Send(TX_BUF,14);		
}   

/*响应写单个寄存器*/ 
void Response_Write_Register(char *buf)
 {
	u16 CRC=0;	
	char TX_BUF[11];
	//待写入寄存器地址
	u16 regAdd = buf[8]<<8|buf[9];
	//待写入数据
	u16 data = (buf[10]<<8)|buf[11];
	 	 
	//回复响应帧
	TX_BUF[0] = buf[0];//帧ID高
	TX_BUF[1] = buf[1];//帧ID低	
	TX_BUF[2] = LocalAddress>>8;//本机地址高
	TX_BUF[3] = LocalAddress;//本机地址低
	TX_BUF[4] = 2;//帧类型
	TX_BUF[5] = buf[5];//目标地址高
	TX_BUF[6] = buf[6];//目标地址低
	TX_BUF[7] = 2;//功能码
	
	if(regAdd>DBUS_REGISTER_LENGTH)
	{
		TX_BUF[8] = 0;//结果	
	}
	else
	{
		TX_BUF[8] = 1;//结果	
		//更新数据
		Dbus_Register[regAdd] = data;
	}
		
	CRC=CRC_CALC(TX_BUF,9);
	
	TX_BUF[9] = CRC>>8;//CRC高
	TX_BUF[10] = CRC;//CRC低	
	
	//发送数据
	Send(TX_BUF,11);	
} 
 
/*响应读多个寄存器*/ 
void Response_Read_Multiple_Registers(char *buf)
{
	u16 CRC=0;	
	u16 Num = buf[10];
	char TX_BUF[DBUS_MAX_LENGTH];//定义数组;
	//待读取寄存器起始地址
	u16 regStartAdd = buf[8]<<8|buf[9];
	
	TX_BUF[0] = buf[0];//帧ID高
	TX_BUF[1] = buf[1];//帧ID低	
	TX_BUF[2] = LocalAddress>>8;//本机地址高
	TX_BUF[3] = LocalAddress;//本机地址低
	TX_BUF[4] = 2;//帧类型
	TX_BUF[5] = buf[5];//目标地址高
	TX_BUF[6] = buf[6];//目标地址低
	TX_BUF[7] = 3;//功能码
	TX_BUF[8] = buf[8];//寄存器起始地址高
	TX_BUF[9] = buf[9];//寄存器起始地址低
	TX_BUF[10] = buf[10];//数量
	
	for(int i=0;i<Num;i++)
	{
		//如果请求地址超出限制，返回数据为0xFFFF
		if((regStartAdd+i)>DBUS_REGISTER_LENGTH)
		{
			TX_BUF[11+i*2] = 0xFF;//数据高
			TX_BUF[12+i*2] = 0xFF;//数据低
		}
		else
		{
			TX_BUF[11+i*2] = Dbus_Register[regStartAdd+i]>>8;//数据高
			TX_BUF[12+i*2] = Dbus_Register[regStartAdd+i];//数据低
		}
	}

	CRC=CRC_CALC(TX_BUF,11+Num*2);
	
	TX_BUF[11+Num*2] = CRC>>8;//CRC高
	TX_BUF[11+Num*2+1] = CRC;//CRC低	
	
	//发送数据
	Send(TX_BUF,11+Num*2+2);
}
 
/*响应写多个寄存器*/ 
void Response_Write_Multiple_Registers(char *buf)
 {
	u16 CRC=0;	
	char TX_BUF[11];
	//待写入寄存器起始地址
	u16 regStartAdd = buf[8]<<8|buf[9];
	u16 Num = buf[10];
	//回复响应帧
	TX_BUF[0] = buf[0];//帧ID高
	TX_BUF[1] = buf[1];//帧ID低	
	TX_BUF[2] = LocalAddress>>8;//本机地址高
	TX_BUF[3] = LocalAddress;//本机地址低
	TX_BUF[4] = 2;//帧类型
	TX_BUF[5] = buf[5];//目标地址高
	TX_BUF[6] = buf[6];//目标地址低
	TX_BUF[7] = 4;//功能码
	TX_BUF[8] = 1;//结果
	for(int i=0;i<Num;i++) 
	{
		if((regStartAdd+i)>DBUS_REGISTER_LENGTH)
		{
			TX_BUF[8] = 0;//结果	
		}
		else
		{
			TX_BUF[8] = 1;//结果	
			//更新数据
			Dbus_Register[regStartAdd+i] = buf[11+i*2]<<8|buf[12+i*2];
		}
	}

	CRC=CRC_CALC(TX_BUF,9);
	
	TX_BUF[9] = CRC>>8;//CRC高
	TX_BUF[10] = CRC;//CRC低	
	
	//发送数据
	Send(TX_BUF,11);		 
 }  




