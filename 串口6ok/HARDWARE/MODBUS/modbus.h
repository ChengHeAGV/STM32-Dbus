#ifndef __MODBUS_H
#define __MODBUS_H	 
#include "sys.h" 
#include "USART2.h"
extern u8 PLC_InPut[];//PLC����״̬�Ĵ���
extern u8 PLC_OutPut[];//PLC���״̬�Ĵ���
extern u16 PLC_Data[];//PLC���״̬�Ĵ���

extern u8 comm_END;
extern u8 Reciver_bit;//����λ��һ


extern u8 USART2_TX_BUF[]; 
extern u8 USART2_RX_BUF[];

void  send_comm(void);
void AnalyzeRecieve(void);
#endif
