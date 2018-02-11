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
#define TASK4_STK_SIZE 		256
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

void Delay(CPU_INT16U s,CPU_INT32U mm,OS_ERR *err)
{
	OSTimeDlyHMSM(0,0,s,mm,OS_OPT_TIME_HMSM_STRICT,err);
}




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
/////////�ر������񣬴���3/////////////////
void task2_task(void *p_arg)
{
	int count = 0;
	while(1)
	{
		count++;
		delay_ms(100);
		u6_printf("test%d\r\n",count);
	}
}

//task3������
void task3_task(void *p_arg)
{
	while(1)
	{
		delay_ms(10);
	}
}

//task4������
void task4_task(void *p_arg)
{
	while(1)
	{
		delay_ms(10);
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