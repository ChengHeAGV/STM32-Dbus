#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "io.h"
#include "led.h"
#include "beep.h"
#include "usart2.h"
#include "usart3.h"
#include "usart6.h"
#include "modbus.h"
#include "includes.h"
#include "adc.h"
#include "w25qxx.h" 
#include "dbus.h" 

//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		256
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define TASK1_TASK_PRIO		4
//任务堆栈大小	
#define TASK1_STK_SIZE 		256
//任务控制块
OS_TCB Task1_TaskTCB;
//任务堆栈	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
void task1_task(void *p_arg);

//任务优先级
#define TASK2_TASK_PRIO		5
//任务堆栈大小	
#define TASK2_STK_SIZE 		256
//任务控制块
OS_TCB Task2_TaskTCB;
//任务堆栈	
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];
//任务函数
void task2_task(void *p_arg);

//任务优先级
#define TASK3_TASK_PRIO		6
//任务堆栈大小	
#define TASK3_STK_SIZE 		256
//任务控制块
OS_TCB Task3_TaskTCB;
//任务堆栈	
CPU_STK TASK3_TASK_STK[TASK3_STK_SIZE];
//任务函数
void task3_task(void *p_arg);

//任务优先级
#define TASK4_TASK_PRIO		7
//任务堆栈大小	
#define TASK4_STK_SIZE 		1024
//任务控制块
OS_TCB Task4_TaskTCB;
//任务堆栈	
CPU_STK TASK4_TASK_STK[TASK4_STK_SIZE];
//任务函数
void task4_task(void *p_arg);

//任务优先级
#define TASK5_TASK_PRIO		8
//任务堆栈大小	
#define TASK5_STK_SIZE 		256
//任务控制块
OS_TCB Task5_TaskTCB;
//任务堆栈	
CPU_STK TASK5_TASK_STK[TASK5_STK_SIZE];
//任务函数
void task5_task(void *p_arg);


//任务优先级
#define TASK6_TASK_PRIO		9
//任务堆栈大小	
#define TASK6_STK_SIZE 		256
//任务控制块
OS_TCB Task6_TaskTCB;
//任务堆栈	
CPU_STK TASK6_TASK_STK[TASK6_STK_SIZE];
//任务函数
void task6_task(void *p_arg);


void WIFI_Send(char *str,u16 len)
{
	USART_OUT(USART6,str,len);
}
void delay()
{
//    OS_ERR *err;
    //OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,err);
    delay_ms(10);
}


//主函数
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);  //时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	
	
	usart1_init(9600);   //串口初始化
	IO_Init(); //IO初始化		
	LED_Init();//led初始化
	BEEP_Init();//蜂鸣器初始化
	usart2_init(9600);
	usart3_init(9600);
	usart6_init(115200);
	TIM3_Int_Init(100-1,8400-1);	//10ms中断一次
	Adc_Init();         //初始化ADC
	W25QXX_Init();//FLASH初始化 
    
  //初始化DBUS
    Dbus.Init(5);
	Dbus.OutPut_interrupt(WIFI_Send);	
	Dbus.Delay_interrupt(delay);

    OSInit(&err);		    //初始化UCOSIII
	OS_CRITICAL_ENTER();	//进入临界区			 
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);      //开启UCOSIII
}
extern 

//开始任务任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif
	
	OS_CRITICAL_ENTER();	//进入临界区
	//创建TASK1任务
	OSTaskCreate((OS_TCB 	* )&Task1_TaskTCB,		
				 (CPU_CHAR	* )"Task1 task", 		
                 (OS_TASK_PTR )task1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK1_TASK_PRIO,     
                 (CPU_STK   * )&TASK1_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK1_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//创建TASK2任务
	OSTaskCreate((OS_TCB 	* )&Task2_TaskTCB,		
				 (CPU_CHAR	* )"task2 task", 		
                 (OS_TASK_PTR )task2_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK2_TASK_PRIO,     	
                 (CPU_STK   * )&TASK2_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK2_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK2_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	

	//创建TASK3任务
	OSTaskCreate((OS_TCB 	* )&Task3_TaskTCB,		
				 (CPU_CHAR	* )"Task3 task", 		
                 (OS_TASK_PTR )task3_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK3_TASK_PRIO,     	
                 (CPU_STK   * )&TASK3_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK3_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK3_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);			 

	//创建TASK4任务
	OSTaskCreate((OS_TCB 	* )&Task4_TaskTCB,		
				 (CPU_CHAR	* )"Task4 task", 		
                 (OS_TASK_PTR )task4_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK4_TASK_PRIO,     	
                 (CPU_STK   * )&TASK4_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK4_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK4_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	

	//创建TASK5任务
	OSTaskCreate((OS_TCB 	* )&Task5_TaskTCB,		
				 (CPU_CHAR	* )"Task5 task", 		
                 (OS_TASK_PTR )task5_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK5_TASK_PRIO,     	
                 (CPU_STK   * )&TASK5_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK5_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK5_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
	//创建TASK6任务
	OSTaskCreate((OS_TCB 	* )&Task6_TaskTCB,		
				 (CPU_CHAR	* )"Task6 task", 		
                 (OS_TASK_PTR )task6_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK6_TASK_PRIO,     	
                 (CPU_STK   * )&TASK6_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK6_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK6_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	

								 
	OS_CRITICAL_EXIT();	//退出临界区
	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}

//src:指令
//dst:返回信息
//timeout:超时时间
int check(char* dst,u16 timeout,char* src,...)
{
	u16 num=0;
	char *resault;
	
	u16 i;
	va_list ap;
	va_start(ap,src);
	vsprintf((char*)USART6_TX_BUF,src,ap);
	va_end(ap);
	i=strlen((const char*)USART6_TX_BUF);//此次发送数据的长度
	
	USART_OUT(USART6,USART6_TX_BUF,i);

	while(num<(timeout/10.0))
	{
		//resault=strstr(USART6_RX_BUF,dst);
		delay_ms(10);
        
		num++;
	}
	memset(USART6_RX_BUF,0,USART6_MAX_RECV_LEN);
	//RxCounter6 = 0;
	if(resault)
	{
		return 1;
	}
	else
		return 0;
}


char* AP="geekiot";
char* PASSWORD="8008208820";
char* HOST_IP="192.168.191.3";
char* HOST_PORT="18666";
//task1任务函数
void task1_task(void *p_arg)
{
	while(1)
	{
		delay_ms(100);
		LED0 = 0;
		delay_ms(100);
		LED0 = 1;
	}
}

//task2任务函数
void task2_task(void *p_arg)
{
    u16 num=0;
	/////////////配置wifi//////////////////
	//等待8266复位
	delay_ms(1000);//ms<1840
	delay_ms(1000);//ms<1840
	u6_printf(" ");
	//AT模式
	//if(check("OK",500,"AT\r\n"))
	//{
		//check("OK",1000,"AT+CWMODE=1\r\n");//配置为STATION模式 no change
		
		//check("OK",15000,"AT+CWJAP=\"%s\",\"%s\"\r\n",AP,PASSWORD);//加入AP
		
		check("OK",1000,"AT+CIPSTART=\"UDP\",\"%s\",%s\r\n",HOST_IP,HOST_PORT);//配置UDP ALREAY CONNECT
		
		check("OK",1000,"AT+CIPMODE=1\r\n");//
		
		check(">",1000,"AT+CIPSEND\r\n");//		
	//}
	////////////////////////////////////////

    Dbus.Register[0]=0x1122;
    Dbus.Register[1]=0x3366;
    Dbus.Register[3]=0xaabb;
	Heart(1);
	while(1)
	{
       num++;
		if(num==5)
		{
			num=0;
           // Heart(0);
            Dbus.Heart(0);
		}
		delay_ms(1000);
       
  
//		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,err);
//        delay_ms(1000);
//        delay_ms(1000);
//        delay_ms(1000);
//				Write_Register(0,1,num);
				Dbus.ReturnMsg = Read_Register(0,1);
	}
}

//task3任务函数
void task3_task(void *p_arg)
{
    int i=0;
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
    delay_ms(1000);
	while(1)
	{
        for(i=0;i<128;i++)
        {
           Dbus.Register[i]=rand();
        }
//        Write_Multiple_Registers(0,0,128,Register);
//        rm = Read_Multiple_Registers(0,0,128);
		delay_ms(1000);
	}
}

//task4任务函数
void task4_task(void *p_arg)
{
	while(1)
	{
    
		 delay_ms(10);
        //收到结束符触发解包函 
        Dbus.OpenBox();
	}
}

//task5任务函数
void task5_task(void *p_arg)
{
	while(1)
	{
		delay_ms(10);
	}
}
//task6任务函数
void task6_task(void *p_arg)
{
	while(1)
	{
		delay_ms(10);
	}
}
