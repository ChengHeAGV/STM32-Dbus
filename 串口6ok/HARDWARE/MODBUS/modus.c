#include "modbus.h"
#define InPutLength 20 //输入、输出和数据寄存器的数量
#define OutPutLength 130 //输入、输出和数据寄存器的数量
#define DataLength 30 //输入、输出和数据寄存器的数量
u8 PLC_InPut[InPutLength];//PLC输入状态寄存器
u8 PLC_OutPut[OutPutLength];//PLC输出状态寄存器
u16 PLC_Data[DataLength];//PLC输出状态寄存器

u8  comm_END = 0;
u8  Reciver_bit = 0;//接收位置一



//extern u8 TxCounter2;
//extern u8 RxCounter2;

/*************************************************
crc16校验码计算函数,计算算法：
1、设置crc校验寄存器的初值为0xffff;
2、将报文的第一个字节与crc寄存器的低字节异或，结果存入crc寄存器
3、判断crc的第一位是否为1，如果是1,crc右移1位后和0xa001异或，如果为0，则再移1位；
4、重复步骤3，直到完成8个位；
5、重复步骤2、3、4直到完成所有字节
6、返回计算结果
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

/*错误返回*/

void errorsend(uint8_t func,uint8_t type)
{
   uint16_t _crc;
   uint8_t crcH,crcL;

   USART2_TX_BUF[0]=0x01;                      //返回站号
   switch(type)
     {
       case 0x08: 
              USART2_TX_BUF[1]=0x80+func;      //返回错误功能码
              USART2_TX_BUF[2]=0x08;           //返回错误代码，08：CRC校验错误
              break;
       case 0x01: 
              USART2_TX_BUF[1]=0x80+func;      //返回错误功能码
              USART2_TX_BUF[2]=0x01;           //返回错误代码，01：功能码错误
              break;
       case 0x02: 
              USART2_TX_BUF[1]=0x80+func;      //返回错误功能码
              USART2_TX_BUF[2]=0x02;           //返回错误代码，02：地址错误
              break;
       case 0x03: 
              USART2_TX_BUF[1]=0x80+func;      //返回错误功能码
              USART2_TX_BUF[2]=0x03;           //返回错误代码，03：数据错误
              break;
       case 0x04: 
              USART2_TX_BUF[0]=0x80+func;      //返回错误功能码
              USART2_TX_BUF[1]=0x04;           //返回错误代码，04：不支持的功能码
              break;     
     }
   _crc=CalcCrc(USART2_TX_BUF,3);
   crcH=(uint8_t)((_crc>>8)&0xff);
   crcL=(uint8_t)(_crc&0xff);
    USART2_TX_BUF[3]=crcL;                      //校验低字节
    USART2_TX_BUF[4]=crcH;                      //校验高字节
    TxCounter2=5;
}

 /*从机响应函数*/

void  send_comm( )
{
		USART_OUT(USART2,USART2_TX_BUF,TxCounter2);
}

/*分析01功能码报文，产生响应报文*/ 

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
   startadd=(startaddH<<8)+startaddL;  //要返回的起始地址               
   bit_num=(bit_numH<<8)+bit_numL;     //要读的字节数量,单位是位
   if((startadd+bit_num)>=OutPutLength)                //最多允许IDLength个位，从第4个位开始读
     {
       errorsend(0x01,0x02);     //响应寄存器数量超出范围
      }
   else
   {

    USART2_TX_BUF[0]=0x01;            //站号
    USART2_TX_BUF[1]=0x01;           //功能码
    if((bit_num%8)==0)
    USART2_TX_BUF[2]=(bit_num)/8;      //要返回的字节数
    else
     USART2_TX_BUF[2]=((bit_num)/8)+1;  //不能整除8的时候要多返回一个字节
    for(i=0;i<USART2_TX_BUF[2];i++)               
     {
       USART2_TX_BUF[3+i]=0;                       //先清零复位
       for(j=0;j<8;j++)                  //每8个位状态组成一个字节返回
       {
         USART2_TX_BUF[3+i]=(uint8_t)((PLC_OutPut[startadd+i*8+j]&0x01)<<j)+USART2_TX_BUF[3+i];     //低位在前，高位在后
         
        }                          
     }
   aakj=CalcCrc(USART2_TX_BUF,(USART2_TX_BUF[2]+3));                 //CRC校验
   USART2_TX_BUF[3+USART2_TX_BUF[2]]=(uint8_t)(aakj&0xff);                   //CRC低字节
   USART2_TX_BUF[4+USART2_TX_BUF[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
   TxCounter2=USART2_TX_BUF[2]+5; 
   }
 } 
      
/*分析02功能码报文，产生响应报文*/ 
//读取plc输入
 void Recirve_02()// 搞定****************************************************
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
   startadd=(startaddH<<8)+startaddL;  //要返回的起始地址               
   bit_num=(bit_numH<<8)+bit_numL;     //要读的字节数量,单位是位
   if((startadd+bit_num)>=InPutLength||startadd>11)   //本案例中只有4个输入位可供读
      {
       errorsend(0x01,0x02);                //响应寄存器数量超出范围
      }
   else
   {
    for(i=0;i<InPutLength;i++)
     PLC_InPut[i]=(~(P3>>i))&0x01;            //先读出输入口的状态
//  for(i=12;i<IDLength;i++)
//  inputBit[i]=0;                              //没有位状态，清零
     

    USART2_TX_BUF[0]=0x01;            //站号
    USART2_TX_BUF[1]=0x02;           //功能码
    if((bit_num%8)==0)
    USART2_TX_BUF[2]=(bit_num)/8;      //要返回的字节数
    else
    USART2_TX_BUF[2]=((bit_num)/8)+1;  //不能整除8的时候要多返回一个字节
    for(i=0;i<USART2_TX_BUF[2];i++)               
     {
       USART2_TX_BUF[3+i]=0;                       //先清零复位
       for(j=0;j<8;j++)                  //每8个位状态组成一个字节返回
       {
         USART2_TX_BUF[3+i]=((PLC_InPut[startadd+i*8+j]&0x01)<<j)+USART2_TX_BUF[3+i];     //低位在前，高位在后
         
        }                          
     }
   aakj=CalcCrc(USART2_TX_BUF,(USART2_TX_BUF[2]+3));                 //CRC校验
   USART2_TX_BUF[3+USART2_TX_BUF[2]]=(uint8_t)(aakj&0xff);                   //CRC低字节
   USART2_TX_BUF[4+USART2_TX_BUF[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
   TxCounter2=USART2_TX_BUF[2]+5; 
   }
 } 
    

/*分析03功能码报文，产生响应报文*/ 
 //多字节返回
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
   startadd=(startaddH<<8)+startaddL;  //要返回的起始地址                 
   length=(lengthH<<8)+lengthL;    //要读的字节数量
   if((startadd+length)>DataLength )                //最多只能返回IDLength个寄存器,64个字节，注意返回的长度不能超过发送数组长度，否则会溢出导致错误
     {
       errorsend(0x03,0x02);     //响应寄存器数量超出范围
      }
   else
   {
    USART2_TX_BUF[0]=0x01;            //站号
    USART2_TX_BUF[1]=0x03;           //功能码
    USART2_TX_BUF[2]=length*2;      //要返回的字节数是请求报文的第五个字节*2
    for(i=0;i<length;i++)
     {
//         USART2_TX_BUF[3+i*2]=I2c_Buf[(startadd+i)*2];   //返回寄存器值的高字节
//         USART2_TX_BUF[4+i*2]=I2c_Buf[(startadd+i)*2+1];        //返回寄存器值得低字节
         USART2_TX_BUF[3+i*2]=(PLC_Data[startadd+i]>>8)&0xff;   //返回寄存器值的高字节
         USART2_TX_BUF[4+i*2]=(PLC_Data[startadd+i])&0xff;        //返回寄存器值得低字节
     }
   aakj=CalcCrc(USART2_TX_BUF,(length*2)+3);                 //CRC校验
   USART2_TX_BUF[3+length*2]=(uint8_t)(aakj&0xff);                   //CRC低字节
   USART2_TX_BUF[4+length*2]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
   TxCounter2=(length*2)+5;    
   }
 }
 
 /*分析04功能码报文，产生响应报文*/ 
 /*这边返回的是输入口的寄存器值*/
//plc输入
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
   startadd=(startaddH<<8)+startaddL;  //要返回的起始地址               
   bit_num=(bit_numH<<8)+bit_numL;     //要读的字节数量,单位是位
   if((startadd+bit_num)>=InPutLength ||startadd>1)//本案例中只有4个输入位1个寄存器可供读
     {
       errorsend(0x01,0x02);     //响应寄存器数量超出范围
      }
   else
   {
    for(i=2;i<6;i++)
//     inputBit[i-2]=(~(P3>>i))&0x01;            //先读出输入口的状态
    for(i=4;i<InPutLength;i++)
    PLC_InPut[i]=0;                              //没有位状态，清零
     

    USART2_TX_BUF[0]=0x01;            //站号
    USART2_TX_BUF[1]=0x04;           //功能码
    USART2_TX_BUF[2]=bit_num*2;
    for(i=0;i<USART2_TX_BUF[2];i++)               
     {
       USART2_TX_BUF[3+i]=0;                       //先清零复位
       for(j=0;j<8;j++)                  //每8个位状态组成一个字节返回
       {
         USART2_TX_BUF[3+i]=(uint8_t)((PLC_InPut[startadd+i*8+j]&0x01)<<j)+USART2_TX_BUF[3+i];     //低位在前，高位在后
         
        }                          
     }
   aakj=CalcCrc(USART2_TX_BUF,(USART2_TX_BUF[2]+3));                 //CRC校验
   USART2_TX_BUF[3+USART2_TX_BUF[2]]=(uint8_t)(aakj&0xff);                   //CRC低字节
   USART2_TX_BUF[4+USART2_TX_BUF[2]]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
   TxCounter2=USART2_TX_BUF[2]+5; 
   }
 }  

/*分析05功能码报文，产生响应报文*/ 
//plc输出
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
   startadd=(startaddH<<8)+startaddL;  //要写入的地址               
   if(startadd>=OutPutLength)               
     {
       errorsend(0x01,0x02);     //响应寄存器数量超出范围
      }
   else
   {
     
     if(bit_valueH==0xff&&bit_valueL==0x00)   //置位线圈
     
         {
            // *(PLC_OutPut+startadd)=1;
             PLC_OutPut[startadd] = 1;
             }
     if(bit_valueH==0x00&&bit_valueL==0x00)   //复位线圈
     {
    // *(PLC_OutPut+startadd)=0;
         PLC_OutPut[startadd] = 0;
     }
  
        USART2_TX_BUF[0]=0x01;            //站号
        USART2_TX_BUF[1]=0x05;           //功能码
        USART2_TX_BUF[2]=startaddH;     //地址高字节
        USART2_TX_BUF[3]=startaddL;     //地址低字节
        USART2_TX_BUF[4]=bit_valueH;     //地址高字节
        USART2_TX_BUF[5]=bit_valueL;     //地址低字节
        aakj=CalcCrc(USART2_TX_BUF,6);                 //CRC校验
        USART2_TX_BUF[6]=(uint8_t)(aakj&0xff);                   //CRC低字节
        USART2_TX_BUF[7]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
        TxCounter2=8; 
   }
 } 

   


/*分析06功能码报文，产生响应报文*/ 
 void Recirve_06()//单字节写入
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
        startadd=(startaddH<<8)+startaddL;  //要写入的起始地址                 
        wdata_06=(wdataH_06<<8)+wdataL_06;             //要写入的数值
  
   if(startadd>DataLength)                    //寄存器地址超出范围
       errorsend(0x06,0x02);         //响应寄存器数量超出范围
  else if(wdata_06>0xFFFF)
       errorsend(0x06,0x03);         //响应数据错误
   else
   {
         
    PLC_Data[startadd]=wdata_06;      //将数值写入寄存器
//        I2c_Buf[startadd*2]=wdata_06>>8;
//        I2c_Buf[startadd*2+1]=wdata_06&0xff;
         
        //写入配置flash
        //FlashWriteStr(FLASH_ADR,sizeof(I2c_Buf),(u8*)&I2c_Buf);

    USART2_TX_BUF[0]=0x01;            //站号
    USART2_TX_BUF[1]=0x06;           //功能码
    USART2_TX_BUF[2]=startaddH;      //返回地址高字节
    USART2_TX_BUF[3]=startaddL;      //返回地址低字节
    USART2_TX_BUF[4]=(uint8_t)(((PLC_Data[startadd])>>8)&0xff);  //返回寄存器值高字节
      USART2_TX_BUF[5]=(uint8_t)(PLC_Data[startadd]&0xff);         //返回寄存器值低字节
    aakj=CalcCrc(USART2_TX_BUF,6);                 //CRC校验
    USART2_TX_BUF[6]=(uint8_t)(aakj&0xff);                   //CRC低字节
    USART2_TX_BUF[7]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
      TxCounter2=8;                                  //返回8个字节长度     
   }
 }

/*分析10功能码报文，产生响应报文*/ 
//多字节接收
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
   startadd=(startaddH<<8)+startaddL;  //要返回的起始地址 
//   register_num=(register_numH<<8)+register_numL;   //寄存器数量                
   length=USART2_RX_BUF[6];                                //要写的字节数量

   if((startadd+(length/2))>DataLength)                //最多允许写IDLength个寄存器
     {
       errorsend(0x10,0x02);     //响应寄存器数量超出范围
      }
   else
   {
    for(i=0;i<(length/2);i++)        //将值写入寄存器
     {
       PLC_Data[startadd+i]=(USART2_RX_BUF[7+i*2]<<8)+((USART2_RX_BUF[8+i*2])&0xff);         
//        I2c_Buf[(startadd+i)*2]=USART2_RX_BUF[7+i*2];
//        I2c_Buf[(startadd+i)*2+1]=USART2_RX_BUF[8+i*2];
     }
    USART2_TX_BUF[0]=0x01;            //站号
    USART2_TX_BUF[1]=0x10;           //功能码
    USART2_TX_BUF[2]=startaddH;      //返回地址高位
    USART2_TX_BUF[3]=startaddL;      //返回地址低位
    USART2_TX_BUF[4]=register_numH;
    USART2_TX_BUF[5]=register_numL;
    aakj=CalcCrc(USART2_TX_BUF,6);                 //CRC校验
    USART2_TX_BUF[6]=(uint8_t)(aakj&0xff);                   //CRC低字节
    USART2_TX_BUF[7]=(uint8_t)((aakj>>8)&0xff);              //CRC高字节
    TxCounter2=8;    
   }
 } 





 


 void AnalyzeRecieve()                              //分析并生成响应报文
 {
      uint16_t _crc;                      
      uint8_t a1,a2;              
     _crc=CalcCrc(USART2_RX_BUF,(RxCounter2-2));  //计算校验CRC
     a1=_crc&0xff;                          //CRC低字节
     a2=(_crc>>8)&0xff;                     //CRC高字节
     if(a1==USART2_RX_BUF[RxCounter2-2]&&a2==USART2_RX_BUF[RxCounter2-1])  //校验正确
      {             
        switch(USART2_RX_BUF[1])
        {
          case 0x01:Recirve_01();break;                    //读位状态 
          case 0x02:Recirve_02();break;                    //读输入位状态 ok                    
          case 0x03:Recirve_03();break;                    //读寄存器
          case 0x04:Recirve_04();break;                    //读输入寄存器
          case 0x05:Recirve_05();break;                    //写单个位状态ok
          case 0x06:Recirve_06();break;                    //写单个寄存器
          case 0x10:Recirve_10();break;                    //写多个寄存器
          default:errorsend(USART2_RX_BUF[1],0x04);break;           //不支持的功能码       
        }      
       } 
     else                                              //返回校验错误代码 
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
