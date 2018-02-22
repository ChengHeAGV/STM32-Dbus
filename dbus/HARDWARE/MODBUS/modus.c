#include "modbus.h"
#define InPutLength 20 //���롢��������ݼĴ���������
#define OutPutLength 130 //���롢��������ݼĴ���������
#define DataLength 30 //���롢��������ݼĴ���������
u8 PLC_InPut[InPutLength];//PLC����״̬�Ĵ���
u8 PLC_OutPut[OutPutLength];//PLC���״̬�Ĵ���
u16 PLC_Data[DataLength];//PLC���״̬�Ĵ���

u8  comm_END = 0;
u8  Reciver_bit = 0;//����λ��һ



//extern u8 TxCounter2;
//extern u8 RxCounter2;

/*************************************************
crc16У������㺯��,�����㷨��
1������crcУ��Ĵ����ĳ�ֵΪ0xffff;
2�������ĵĵ�һ���ֽ���crc�Ĵ����ĵ��ֽ���򣬽������crc�Ĵ���
3���ж�crc�ĵ�һλ�Ƿ�Ϊ1�������1,crc����1λ���0xa001������Ϊ0��������1λ��
4���ظ�����3��ֱ�����8��λ��
5���ظ�����2��3��4ֱ����������ֽ�
6�����ؼ�����
***********************************************/
uint16_t CalcCrc(unsigned char *chData,unsigned short uNo)
{
    uint16_t crc=0xffff;
    uint16_t i,j;
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

/*���󷵻�*/

void errorsend(uint8_t func,uint8_t type)
{
   uint16_t _crc;
   uint8_t crcH,crcL;

   USART2_TX_BUF[0]=0x01;                      //����վ��
   switch(type)
     {
       case 0x08: 
              USART2_TX_BUF[1]=0x80+func;      //���ش�������
              USART2_TX_BUF[2]=0x08;           //���ش�����룬08��CRCУ�����
              break;
       case 0x01: 
              USART2_TX_BUF[1]=0x80+func;      //���ش�������
              USART2_TX_BUF[2]=0x01;           //���ش�����룬01�����������
              break;
       case 0x02: 
              USART2_TX_BUF[1]=0x80+func;      //���ش�������
              USART2_TX_BUF[2]=0x02;           //���ش�����룬02����ַ����
              break;
       case 0x03: 
              USART2_TX_BUF[1]=0x80+func;      //���ش�������
              USART2_TX_BUF[2]=0x03;           //���ش�����룬03�����ݴ���
              break;
       case 0x04: 
              USART2_TX_BUF[0]=0x80+func;      //���ش�������
              USART2_TX_BUF[1]=0x04;           //���ش�����룬04����֧�ֵĹ�����
              break;     
     }
   _crc=CalcCrc(USART2_TX_BUF,3);
   crcH=(uint8_t)((_crc>>8)&0xff);
   crcL=(uint8_t)(_crc&0xff);
    USART2_TX_BUF[3]=crcL;                      //У����ֽ�
    USART2_TX_BUF[4]=crcH;                      //У����ֽ�
    TxCounter2=5;
}

 /*�ӻ���Ӧ����*/

void  send_comm( )
{
		USART_OUT(USART2,USART2_TX_BUF,TxCounter2);
}

/*����01�����뱨�ģ�������Ӧ����*/ 

 void Recirve_01()
 {
   
   uint16_t startadd;
   uint16_t bit_num;
   uint8_t startaddH,startaddL;
   uint8_t bit_numH,bit_numL;
   uint16_t i,j;
   uint16_t aakj;

    startaddH=USART2_RX_BUF[2];
    startaddL=USART2_RX_BUF[3];
    bit_numH=USART2_RX_BUF[4];
    bit_numL=USART2_RX_BUF[5];
   startadd=(startaddH<<8)+startaddL;  //Ҫ���ص���ʼ��ַ               
   bit_num=(bit_numH<<8)+bit_numL;     //Ҫ�����ֽ�����,��λ��λ
   if((startadd+bit_num)>=OutPutLength)                //�������IDLength��λ���ӵ�4��λ��ʼ��
     {
       errorsend(0x01,0x02);     //��Ӧ�Ĵ�������������Χ
      }
   else
   {

    USART2_TX_BUF[0]=0x01;            //վ��
    USART2_TX_BUF[1]=0x01;           //������
    if((bit_num%8)==0)
    USART2_TX_BUF[2]=(bit_num)/8;      //Ҫ���ص��ֽ���
    else
     USART2_TX_BUF[2]=((bit_num)/8)+1;  //��������8��ʱ��Ҫ�෵��һ���ֽ�
    for(i=0;i<USART2_TX_BUF[2];i++)               
     {
       USART2_TX_BUF[3+i]=0;                       //�����㸴λ
       for(j=0;j<8;j++)                  //ÿ8��λ״̬���һ���ֽڷ���
       {
         USART2_TX_BUF[3+i]=(uint8_t)((PLC_OutPut[startadd+i*8+j]&0x01)<<j)+USART2_TX_BUF[3+i];     //��λ��ǰ����λ�ں�
         
        }                          
     }
   aakj=CalcCrc(USART2_TX_BUF,(USART2_TX_BUF[2]+3));                 //CRCУ��
   USART2_TX_BUF[3+USART2_TX_BUF[2]]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
   USART2_TX_BUF[4+USART2_TX_BUF[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
   TxCounter2=USART2_TX_BUF[2]+5; 
   }
 } 
      
/*����02�����뱨�ģ�������Ӧ����*/ 
//��ȡplc����
 void Recirve_02()// �㶨****************************************************
 {

   uint16_t startadd;
   uint16_t bit_num;
   uint8_t startaddH,startaddL;
   uint8_t bit_numH,bit_numL;
   uint16_t i,j;
   uint16_t aakj;
     
     uint16_t P3= 0xffff;
     
     for(i=0;i<InPutLength ;i++)
     {
         if(PLC_InPut[i])
            P3 = P3-(0x01<<(i));
        }
//   if(!OK_Key)P3 =0xffff-(0x01<<(10-1));
//   else
//       P3 =0xffff;
     
   startaddH=USART2_RX_BUF[2];
   startaddL=USART2_RX_BUF[3];
   bit_numH=USART2_RX_BUF[4];
   bit_numL=USART2_RX_BUF[5];
   startadd=(startaddH<<8)+startaddL;  //Ҫ���ص���ʼ��ַ               
   bit_num=(bit_numH<<8)+bit_numL;     //Ҫ�����ֽ�����,��λ��λ
   if((startadd+bit_num)>=InPutLength||startadd>11)   //��������ֻ��4������λ�ɹ���
      {
       errorsend(0x01,0x02);                //��Ӧ�Ĵ�������������Χ
      }
   else
   {
    for(i=0;i<InPutLength;i++)
     PLC_InPut[i]=(~(P3>>i))&0x01;            //�ȶ�������ڵ�״̬
//  for(i=12;i<IDLength;i++)
//  inputBit[i]=0;                              //û��λ״̬������
     

    USART2_TX_BUF[0]=0x01;            //վ��
    USART2_TX_BUF[1]=0x02;           //������
    if((bit_num%8)==0)
    USART2_TX_BUF[2]=(bit_num)/8;      //Ҫ���ص��ֽ���
    else
    USART2_TX_BUF[2]=((bit_num)/8)+1;  //��������8��ʱ��Ҫ�෵��һ���ֽ�
    for(i=0;i<USART2_TX_BUF[2];i++)               
     {
       USART2_TX_BUF[3+i]=0;                       //�����㸴λ
       for(j=0;j<8;j++)                  //ÿ8��λ״̬���һ���ֽڷ���
       {
         USART2_TX_BUF[3+i]=((PLC_InPut[startadd+i*8+j]&0x01)<<j)+USART2_TX_BUF[3+i];     //��λ��ǰ����λ�ں�
         
        }                          
     }
   aakj=CalcCrc(USART2_TX_BUF,(USART2_TX_BUF[2]+3));                 //CRCУ��
   USART2_TX_BUF[3+USART2_TX_BUF[2]]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
   USART2_TX_BUF[4+USART2_TX_BUF[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
   TxCounter2=USART2_TX_BUF[2]+5; 
   }
 } 
    

/*����03�����뱨�ģ�������Ӧ����*/ 
 //���ֽڷ���
 void Recirve_03()
 {

   uint16_t startadd;
   uint16_t length;
   uint8_t startaddH,startaddL;
   uint8_t lengthH,lengthL;
   uint16_t i;
   uint16_t aakj;
     
    startaddH=USART2_RX_BUF[2];
    startaddL=USART2_RX_BUF[3];
    lengthH=USART2_RX_BUF[4];
    lengthL=USART2_RX_BUF[5];
   startadd=(startaddH<<8)+startaddL;  //Ҫ���ص���ʼ��ַ                 
   length=(lengthH<<8)+lengthL;    //Ҫ�����ֽ�����
   if((startadd+length)>DataLength )                //���ֻ�ܷ���IDLength���Ĵ���,64���ֽڣ�ע�ⷵ�صĳ��Ȳ��ܳ����������鳤�ȣ������������´���
     {
       errorsend(0x03,0x02);     //��Ӧ�Ĵ�������������Χ
      }
   else
   {
    USART2_TX_BUF[0]=0x01;            //վ��
    USART2_TX_BUF[1]=0x03;           //������
    USART2_TX_BUF[2]=length*2;      //Ҫ���ص��ֽ����������ĵĵ�����ֽ�*2
    for(i=0;i<length;i++)
     {
//         USART2_TX_BUF[3+i*2]=I2c_Buf[(startadd+i)*2];   //���ؼĴ���ֵ�ĸ��ֽ�
//         USART2_TX_BUF[4+i*2]=I2c_Buf[(startadd+i)*2+1];        //���ؼĴ���ֵ�õ��ֽ�
         USART2_TX_BUF[3+i*2]=(PLC_Data[startadd+i]>>8)&0xff;   //���ؼĴ���ֵ�ĸ��ֽ�
         USART2_TX_BUF[4+i*2]=(PLC_Data[startadd+i])&0xff;        //���ؼĴ���ֵ�õ��ֽ�
     }
   aakj=CalcCrc(USART2_TX_BUF,(length*2)+3);                 //CRCУ��
   USART2_TX_BUF[3+length*2]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
   USART2_TX_BUF[4+length*2]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
   TxCounter2=(length*2)+5;    
   }
 }
 
 /*����04�����뱨�ģ�������Ӧ����*/ 
 /*��߷��ص�������ڵļĴ���ֵ*/
//plc����
 void Recirve_04()
 {

   uint16_t startadd;
   uint16_t bit_num;
   uint8_t startaddH,startaddL;
   uint8_t bit_numH,bit_numL;
   uint16_t i,j;
   uint16_t aakj;
   startaddH=USART2_RX_BUF[2];
   startaddL=USART2_RX_BUF[3];
   bit_numH=USART2_RX_BUF[4];
   bit_numL=USART2_RX_BUF[5];
   startadd=(startaddH<<8)+startaddL;  //Ҫ���ص���ʼ��ַ               
   bit_num=(bit_numH<<8)+bit_numL;     //Ҫ�����ֽ�����,��λ��λ
   if((startadd+bit_num)>=InPutLength ||startadd>1)//��������ֻ��4������λ1���Ĵ����ɹ���
     {
       errorsend(0x01,0x02);     //��Ӧ�Ĵ�������������Χ
      }
   else
   {
    for(i=2;i<6;i++)
//     inputBit[i-2]=(~(P3>>i))&0x01;            //�ȶ�������ڵ�״̬
    for(i=4;i<InPutLength;i++)
    PLC_InPut[i]=0;                              //û��λ״̬������
     

    USART2_TX_BUF[0]=0x01;            //վ��
    USART2_TX_BUF[1]=0x04;           //������
    USART2_TX_BUF[2]=bit_num*2;
    for(i=0;i<USART2_TX_BUF[2];i++)               
     {
       USART2_TX_BUF[3+i]=0;                       //�����㸴λ
       for(j=0;j<8;j++)                  //ÿ8��λ״̬���һ���ֽڷ���
       {
         USART2_TX_BUF[3+i]=(uint8_t)((PLC_InPut[startadd+i*8+j]&0x01)<<j)+USART2_TX_BUF[3+i];     //��λ��ǰ����λ�ں�
         
        }                          
     }
   aakj=CalcCrc(USART2_TX_BUF,(USART2_TX_BUF[2]+3));                 //CRCУ��
   USART2_TX_BUF[3+USART2_TX_BUF[2]]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
   USART2_TX_BUF[4+USART2_TX_BUF[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
   TxCounter2=USART2_TX_BUF[2]+5; 
   }
 }  

/*����05�����뱨�ģ�������Ӧ����*/ 
//plc���
 void Recirve_05()
 {

   uint16_t startadd;
   uint8_t startaddH,startaddL;
   uint8_t bit_valueH,bit_valueL;
   uint16_t aakj;
    startaddH=USART2_RX_BUF[2];
    startaddL=USART2_RX_BUF[3];
    bit_valueH=USART2_RX_BUF[4];
    bit_valueL=USART2_RX_BUF[5];
   startadd=(startaddH<<8)+startaddL;  //Ҫд��ĵ�ַ               
   if(startadd>=OutPutLength)               
     {
       errorsend(0x01,0x02);     //��Ӧ�Ĵ�������������Χ
      }
   else
   {
     
     if(bit_valueH==0xff&&bit_valueL==0x00)   //��λ��Ȧ
     
         {
            // *(PLC_OutPut+startadd)=1;
             PLC_OutPut[startadd] = 1;
             }
     if(bit_valueH==0x00&&bit_valueL==0x00)   //��λ��Ȧ
     {
    // *(PLC_OutPut+startadd)=0;
         PLC_OutPut[startadd] = 0;
     }
  
        USART2_TX_BUF[0]=0x01;            //վ��
        USART2_TX_BUF[1]=0x05;           //������
        USART2_TX_BUF[2]=startaddH;     //��ַ���ֽ�
        USART2_TX_BUF[3]=startaddL;     //��ַ���ֽ�
        USART2_TX_BUF[4]=bit_valueH;     //��ַ���ֽ�
        USART2_TX_BUF[5]=bit_valueL;     //��ַ���ֽ�
        aakj=CalcCrc(USART2_TX_BUF,6);                 //CRCУ��
        USART2_TX_BUF[6]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
        USART2_TX_BUF[7]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
        TxCounter2=8; 
   }
 } 

   


/*����06�����뱨�ģ�������Ӧ����*/ 
 void Recirve_06()//���ֽ�д��
 {

        uint16_t startadd;
        uint16_t wdata_06;
        uint8_t startaddH,startaddL;
        uint8_t wdataH_06,wdataL_06;
        uint16_t aakj;
        startaddH=USART2_RX_BUF[2];
        startaddL=USART2_RX_BUF[3];
        wdataH_06=USART2_RX_BUF[4];
        wdataL_06=USART2_RX_BUF[5];
        startadd=(startaddH<<8)+startaddL;  //Ҫд�����ʼ��ַ                 
        wdata_06=(wdataH_06<<8)+wdataL_06;             //Ҫд�����ֵ
  
   if(startadd>DataLength)                    //�Ĵ�����ַ������Χ
       errorsend(0x06,0x02);         //��Ӧ�Ĵ�������������Χ
  else if(wdata_06>0xFFFF)
       errorsend(0x06,0x03);         //��Ӧ���ݴ���
   else
   {
         
    PLC_Data[startadd]=wdata_06;      //����ֵд��Ĵ���
//        I2c_Buf[startadd*2]=wdata_06>>8;
//        I2c_Buf[startadd*2+1]=wdata_06&0xff;
         
        //д������flash
        //FlashWriteStr(FLASH_ADR,sizeof(I2c_Buf),(u8*)&I2c_Buf);

    USART2_TX_BUF[0]=0x01;            //վ��
    USART2_TX_BUF[1]=0x06;           //������
    USART2_TX_BUF[2]=startaddH;      //���ص�ַ���ֽ�
    USART2_TX_BUF[3]=startaddL;      //���ص�ַ���ֽ�
    USART2_TX_BUF[4]=(uint8_t)(((PLC_Data[startadd])>>8)&0xff);  //���ؼĴ���ֵ���ֽ�
      USART2_TX_BUF[5]=(uint8_t)(PLC_Data[startadd]&0xff);         //���ؼĴ���ֵ���ֽ�
    aakj=CalcCrc(USART2_TX_BUF,6);                 //CRCУ��
    USART2_TX_BUF[6]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
    USART2_TX_BUF[7]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
      TxCounter2=8;                                  //����8���ֽڳ���     
   }
 }

/*����10�����뱨�ģ�������Ӧ����*/ 
//���ֽڽ���
 void Recirve_10()
 {
   uint16_t startadd;  
//   uint16_t register_num;
   uint8_t startaddH,startaddL;
   uint8_t register_numH,register_numL;
   uint8_t length;
   uint16_t i;
   uint16_t aakj;
    startaddH=USART2_RX_BUF[2];
    startaddL=USART2_RX_BUF[3];
    register_numH=USART2_RX_BUF[4];
    register_numL=USART2_RX_BUF[5];
   startadd=(startaddH<<8)+startaddL;  //Ҫ���ص���ʼ��ַ 
//   register_num=(register_numH<<8)+register_numL;   //�Ĵ�������                
   length=USART2_RX_BUF[6];                                //Ҫд���ֽ�����

   if((startadd+(length/2))>DataLength)                //�������дIDLength���Ĵ���
     {
       errorsend(0x10,0x02);     //��Ӧ�Ĵ�������������Χ
      }
   else
   {
    for(i=0;i<(length/2);i++)        //��ֵд��Ĵ���
     {
       PLC_Data[startadd+i]=(USART2_RX_BUF[7+i*2]<<8)+((USART2_RX_BUF[8+i*2])&0xff);         
//        I2c_Buf[(startadd+i)*2]=USART2_RX_BUF[7+i*2];
//        I2c_Buf[(startadd+i)*2+1]=USART2_RX_BUF[8+i*2];
     }
    USART2_TX_BUF[0]=0x01;            //վ��
    USART2_TX_BUF[1]=0x10;           //������
    USART2_TX_BUF[2]=startaddH;      //���ص�ַ��λ
    USART2_TX_BUF[3]=startaddL;      //���ص�ַ��λ
    USART2_TX_BUF[4]=register_numH;
    USART2_TX_BUF[5]=register_numL;
    aakj=CalcCrc(USART2_TX_BUF,6);                 //CRCУ��
    USART2_TX_BUF[6]=(uint8_t)(aakj&0xff);                   //CRC���ֽ�
    USART2_TX_BUF[7]=(uint8_t)((aakj>>8)&0xff);              //CRC���ֽ�
    TxCounter2=8;    
   }
 } 





 


 void AnalyzeRecieve()                              //������������Ӧ����
 {
      uint16_t _crc;                      
      uint8_t a1,a2;              
     _crc=CalcCrc(USART2_RX_BUF,(RxCounter2-2));  //����У��CRC
     a1=_crc&0xff;                          //CRC���ֽ�
     a2=(_crc>>8)&0xff;                     //CRC���ֽ�
     if(a1==USART2_RX_BUF[RxCounter2-2]&&a2==USART2_RX_BUF[RxCounter2-1])  //У����ȷ
      {             
        switch(USART2_RX_BUF[1])
        {
          case 0x01:Recirve_01();break;                    //��λ״̬ 
          case 0x02:Recirve_02();break;                    //������λ״̬ ok                    
          case 0x03:Recirve_03();break;                    //���Ĵ���
          case 0x04:Recirve_04();break;                    //������Ĵ���
          case 0x05:Recirve_05();break;                    //д����λ״̬ok
          case 0x06:Recirve_06();break;                    //д�����Ĵ���
          case 0x10:Recirve_10();break;                    //д����Ĵ���
          default:errorsend(USART2_RX_BUF[1],0x04);break;           //��֧�ֵĹ�����       
        }      
       } 
     else                                              //����У�������� 
       {
         switch(USART2_RX_BUF[1])
           {
           case 0x01:errorsend(0x01,0x08);break; 
             case 0x02:errorsend(0x02,0x08);break;
             case 0x03:errorsend(0x03,0x08);break;
             case 0x04:errorsend(0x04,0x08);break;
             case 0x05:errorsend(0x05,0x08);break;
             case 0x06:errorsend(0x06,0x08);break;
             case 0x10:errorsend(0x10,0x08);break;                 
           }       
         } 
}
