#ifndef __USART6_H
#define __USART6_H	 
#include "sys.h"  

#define USART6_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define USART6_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define USART6_RX_EN 			1					//0,������;1,����.
extern u8 TxCounter6;
extern u8 RxCounter6;

extern char  USART6_RX_BUF[USART6_MAX_RECV_LEN]; 		//���ջ���,���USART6_MAX_RECV_LEN�ֽ�
extern char  USART6_TX_BUF[USART6_MAX_SEND_LEN]; 		//���ͻ���,���USART6_MAX_SEND_LEN�ֽ�
extern u16 USART6_RX_STA;   						//��������״̬

void usart6_init(u32 bound);				//����6��ʼ�� 
void u6_printf(char* fmt, ...);
#endif













