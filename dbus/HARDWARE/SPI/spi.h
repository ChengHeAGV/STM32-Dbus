#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
    													  
void SPI2_Init(void);			 //��ʼ��SPI1��
void SPI2_SetSpeed(u8 SpeedSet); //����SPI1�ٶ�   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI1���߶�дһ���ֽ�
		 
#endif
