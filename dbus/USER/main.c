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

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		256
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define TASK1_TASK_PRIO		4
//�����ջ��С	
#define TASK1_STK_SIZE 		256
//������ƿ�
OS_TCB Task1_TaskTCB;
//�����ջ	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
void task1_task(void *p_arg);

//�������ȼ�
#define TASK2_TASK_PRIO		5
//�����ջ��С	
#define TASK2_STK_SIZE 		256
//������ƿ�
OS_TCB Task2_TaskTCB;
//�����ջ	
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];
//������
void task2_task(void *p_arg);

//�������ȼ�
#define TASK3_TASK_PRIO		6
//�����ջ��С	
#define TASK3_STK_SIZE 		256
//������ƿ�
OS_TCB Task3_TaskTCB;
//�����ջ	
CPU_STK TASK3_TASK_STK[TASK3_STK_SIZE];
//������
void task3_task(void *p_arg);

//�������ȼ�
#define TASK4_TASK_PRIO		7
//�����ջ��С	
#define TASK4_STK_SIZE 		1024
//������ƿ�
OS_TCB Task4_TaskTCB;
//�����ջ	
CPU_STK TASK4_TASK_STK[TASK4_STK_SIZE];
//������
void task4_task(void *p_arg);

//�������ȼ�
#define TASK5_TASK_PRIO		8
//�����ջ��С	
#define TASK5_STK_SIZE 		256
//������ƿ�
OS_TCB Task5_TaskTCB;
//�����ջ	
CPU_STK TASK5_TASK_STK[TASK5_STK_SIZE];
//������
void task5_task(void *p_arg);


//�������ȼ�
#define TASK6_TASK_PRIO		9
//�����ջ��С	
#define TASK6_STK_SIZE 		256
//������ƿ�
OS_TCB Task6_TaskTCB;
//�����ջ	
CPU_STK TASK6_TASK_STK[TASK6_STK_SIZE];
//������
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


//������
int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);  //ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	
	
	usart1_init(9600);   //���ڳ�ʼ��
	IO_Init(); //IO��ʼ��		
	LED_Init();//led��ʼ��
	BEEP_Init();//��������ʼ��
	usart2_init(9600);
	usart3_init(9600);
	usart6_init(115200);
	TIM3_Int_Init(100-1,8400-1);	//10ms�ж�һ��
	Adc_Init();         //��ʼ��ADC
	W25QXX_Init();//FLASH��ʼ�� 
    
  //��ʼ��DBUS
    Dbus.Init(5);
	Dbus.OutPut_interrupt(WIFI_Send);	
	Dbus.Delay_interrupt(delay);

    OSInit(&err);		    //��ʼ��UCOSIII
	OS_CRITICAL_ENTER();	//�����ٽ���			 
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);      //����UCOSIII
}
extern 

//��ʼ����������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	//����TASK1����
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
				 
	//����TASK2����
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

	//����TASK3����
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

	//����TASK4����
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

	//����TASK5����
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
	//����TASK6����
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

								 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
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
	vsprintf((char*)USART6_TX_BUF,src,ap);
	va_end(ap);
	i=strlen((const char*)USART6_TX_BUF);//�˴η������ݵĳ���
	
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
//task1������
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

//task2������
void task2_task(void *p_arg)
{
    u16 num=0;
	/////////////����wifi//////////////////
	//�ȴ�8266��λ
	delay_ms(1000);//ms<1840
	delay_ms(1000);//ms<1840
	u6_printf(" ");
	//ATģʽ
	//if(check("OK",500,"AT\r\n"))
	//{
		//check("OK",1000,"AT+CWMODE=1\r\n");//����ΪSTATIONģʽ no change
		
		//check("OK",15000,"AT+CWJAP=\"%s\",\"%s\"\r\n",AP,PASSWORD);//����AP
		
		check("OK",1000,"AT+CIPSTART=\"UDP\",\"%s\",%s\r\n",HOST_IP,HOST_PORT);//����UDP ALREAY CONNECT
		
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

//task3������
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

//task4������
void task4_task(void *p_arg)
{
	while(1)
	{
    
		 delay_ms(10);
        //�յ���������������� 
        Dbus.OpenBox();
	}
}

//task5������
void task5_task(void *p_arg)
{
	while(1)
	{
		delay_ms(10);
	}
}
//task6������
void task6_task(void *p_arg)
{
	while(1)
	{
		delay_ms(10);
	}
}
