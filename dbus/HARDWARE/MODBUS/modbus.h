#ifndef __MODBUS_H
#define __MODBUS_H	 
#include "sys.h" 
#include "USART2.h"
extern u8 PLC_InPut[];//PLC输入状态寄存器
extern u8 PLC_OutPut[];//PLC输出状态寄存器
extern u16 PLC_Data[];//PLC输出状态寄存器

extern u8 comm_END;
extern u8 Reciver_bit;//接收位置一


extern u8 USART2_TX_BUF[]; 
extern u8 USART2_RX_BUF[];

void  send_comm(void);
void AnalyzeRecieve(void);
#endif
