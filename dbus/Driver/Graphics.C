#include "graphics.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stm32f4xx.h>

#include "RA8875.h"
#include "spi_flash.h"
//#define LCDBUFSIZE	800*480
#define LCDW 480
#define LCDH 272

extern void DelayMs(u32 nMs);
extern const struct __ASC_ZK sysEnzk[];
extern const struct __HZK_ZK sysHzzk[];
extern u8 TOUCHSTS;
u16 fontbuf[256];
struct 
{
u8 R;
u8 G;
u8 B;
u8 RT;
u8 GT;
u8 BT;
u8 RA;
u8 GA;
u8 BA;
u16 RStartPix;
u16 REndPix;
u16 GStartPix;
u16 GEndPix;
u16 BStartPix;
u16 BEndPix;
u16 BarStartPix;
u16 BarLenth;
}RGBGradient;

//////////////////////////////////////////////常用函数////////////////////////////////////////                      
//code 字符指针开始
//从字库中查找出字模首地址
//code 字符串的开始地址,ascii码
//mat  数据存放地址 size*2 bytes大小
//正点原子@HYW
//CHECK:09/10/30
/***************************************************************
GBK机器码转换为区位码

***************************************************************/
void GetGBKData(unsigned char *code,unsigned char *Data)
{
	unsigned char ch,cl;
	unsigned char i;					  
	unsigned long foffset; 
	ch=*code;
	cl=*(++code);
	if(ch<0x81||cl<0x40||cl==0xff||ch==0xff)
	{   		    
	  for(i=0;i<(16*2);i++)
			*Data++=0x00;
	  return; 
	}          
	if(cl<0x7f)cl-=0x40;//注意!
	else cl-=0x41;
	ch-=0x81;   
	foffset = STARTADDR + ((unsigned long)190*ch+cl)*32;//得到字库中的字节偏移量  		  
	SPI_Flash_Read(Data,foffset,32);
} 
/***************************************************************
GB2321机器码转换为区位码
***************************************************************/
void Get_HzMat1(unsigned char *code,unsigned char *Data)
{
	unsigned char ch,cl;
	unsigned long foffset; 
	ch=*code;
	cl=*(++code);
	foffset = STARTADDR + ((ch-0xa0-1)*94+(cl-0xa0-1))*32;
	SPI_Flash_Read(Data,foffset,32);

} 


/**************************************************************************************
函数功能：清除整个LCD屏幕
输入：    目标颜色
输出：	  无
**************************************************************************************/
void LcdClear(u16 color)
{
	Text_color(color); // 设定颜色
	Geometric_Coordinate(0,479,0,271); // 设定矩形坐标
	WriteCommand(0x90);
	WriteData(0xB0);
	RA8875_WAITSTATUS();
}

/**************************************************************************************
函数功能：画点
输入：    u16  x,y    	        点的坐标
	  	  u16  color		颜色
输出：	  无


**************************************************************************************/
void LcdPrintDot(u16 x, u16 y, u16 color)
{
	XY_Coordinate(x,y);
	WriteCommand(0x02);//MRWC
	WriteData(color);
}
/**************************************************************************************
函数功能：在LCD上打印一个字符
输入：    ch       	字符
          x,y	      显示位置坐标
					color	    字体颜色
					bcolor    背景颜色
输出：	  无
**************************************************************************************/
void LcdPrintchar(char ch,u16 x,u16 y,u16 color,u16 bcolor,u8 mode)
{
	u16 px;
	
	px = x;
	Text_Mode();
	if(mode == 1)
		Font_with_BackgroundTransparency();
	else
		Font_with_BackgroundColor();
	//Horizontal_FontEnlarge_x2();
	//Vertical_FontEnlarge_x2();
	Text_Background_Color(bcolor);
	Text_Foreground_Color(color);
	Font_Coordinate(px,y);//坐标
	Text_Mode();
	WriteCommand(0x02);
	WriteData(ch);
	px += 8;
	Graphic_Mode();//切换回图形模式
}
/**************************************************************************************
函数功能：在LCD上打印一个汉字
输入：    *font      汉字指针
          x,y	       显示位置坐标
					color	     字体颜色
					bcolor     背景颜色
					mode			 模式，1-透明0-非透明
输出：	  无
备注：		此字库是按列自上到下，然后从左到右的顺序取模
**************************************************************************************/
void LcdPrintHz(u8 *font,u16 x,u16 y,u16 color,u16 bcolor,u8 mode)
{
	u16 i,j,fcode;
	u8 dzk1[32];	
	
	GetGBKData(font,dzk1);//得到相应大小的点阵数据
	for(i=0;i<16;i++)
	{
		fcode = dzk1[i*2]<<8 | dzk1[(i*2+1)];
		Memory_Write_TopDown_LeftRight();
			XY_Coordinate(x+i,y);
		for(j=0;j<16;j++)//生成一列的数据
		{	
			WriteCommand(0x02);
			if(fcode&(0x8000>>j))
			{
				WriteData(color);
			}
			else
			{
					WriteData(bcolor);

				
			}
		}
	}
}

/**************************************************************************************
功能描述: 	在屏幕显示一个字符串
输    入: 	char* str     字符串指针
            u16 x,y				目标显示位置坐标
						u16 color			字体颜色
						u16 bcolor		背景颜色
						u8  mode    	模式，1-透明0-非透明
输    出: 	无
**************************************************************************************/
void LcdPrintStr(char *str,u16 x,u16 y,u16 color,u16 bcolor,u8 mode)
{
	u16 px;
	px = x;
	while(*str != 0)
	{
		if(*str > 0x7F)//是汉字
		{
			if(px > 480 - 16)
			{
				px = 0;
				y += 16;
			}
			LcdPrintHz(str,px,y,color,bcolor,mode);
			str += 2;
			px += 16;
		}
		else//非汉字
		{
			if(px > 480 - 12)
			{
				px = 0;
				y += 16;
			}
			LcdPrintchar(*str,px,y,color,bcolor,mode);
			str++;
			px += 8;
		}
		RA8875_WAITSTATUS();
	}
}

void LcdPrintStrMid(u8 *str,u8 x,u16 y,u8 pixlen)
{
	u16 strpix , movpix;
   	strpix = strlen(str)*12;
	if(strpix > pixlen)
		LcdPrintStr(str,x,y,0XF800,0x0,1);
	else
	{
		movpix = (pixlen-strpix)/2;
	  LcdPrintStr(str,movpix+x,y,0XF800,0x0,1);
	}
} 
/**************************************************************************************
功能描述: 在屏幕按格式显示一个字符串
输    入: 

输    出: 无
**************************************************************************************/
void LcdPrintf(u16 x,u16 y,u16 color,u16 bcolor,u8 mode,char *fmt,...)
{
    va_list ap;
    char str[64];

    va_start(ap,fmt);
    vsprintf(str,fmt,ap);
    LcdPrintStr(str,x,y,color,bcolor,mode);
    va_end(ap);
}
/**************************************************************************************
函数功能：	画水平直线
输入	：WORD  x    		起点的横坐标
	  		WORD  y  	    起点的纵坐标
	  		WORD  width		直线长度
	  		WORD  color		颜色
输出	：	无
**************************************************************************************/
void LcdGeometryHorz(u16 x, u16 y, u16 width, u16 color)
{
	Text_color(color); // 设定颜色
	Geometric_Coordinate(x,x+width-1,y,y); // 设置水平线起始点
	WriteCommand(0x90);//写0x90寄存器
	WriteData(0x80);   //向0x90寄存器写数据
	RA8875_WAITSTATUS();
}
/**************************************************************************************
函数功能：画两条紧挨着的线
**************************************************************************************/
void LcdGeometryHorzD(u16 x, u16 y, u16 width, u16 color)
{
		LcdGeometryHorz(x, y, width, color);
		LcdGeometryHorz(x, y+1, width, color);
}
/**************************************************************************************
函数功能：  画垂直直线
输入	：  u16  x    	    起点的横坐标
	  		u16  y  	    起点的纵坐标
	  		u16  height		直线高度
	  		u16  color		颜色
输出	：	无
**************************************************************************************/
void LcdGeometryVertical(u16 x, u16 y, u16 height, u16 color)
{
	Text_color(color); //设定颜色
	Geometric_Coordinate(x,x,y,y+height-1); // 设置起始点坐标
	WriteCommand(0x90);
	WriteData(0x80);
	RA8875_WAITSTATUS();
}
/**************************************************************************************
函数功能：画一条斜线
输入：    u16  x1   起点的横坐标
	  u16  y1  	    起点的纵坐标
	  u16  x2		结束的横坐标
	  u16  y2		结束的纵坐标
	  u16  color		颜色
输出：	  无
**************************************************************************************/
void LcdGeometryBIAS(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	Text_color(color); // 设定颜色
	Geometric_Coordinate(x1,x2,y1,y2); // 设置起始点坐标
	WriteCommand(0x90);
	WriteData(0x80);
	RA8875_WAITSTATUS();
}
/**************************************************************************************
函数功能：画矩形
输入：    矩形的左上角和右下角坐标，和目标颜色
		  x1,y1  左上角坐标
		  x2,y2  右上角坐标
		  fill	 0  画矩形框
		  		 1  矩形填充
输出：	  无
**************************************************************************************/
void LcdGeometryFillRec(u16 x1, u16 y1, u16 x2, u16 y2, u8 fill, u16 color)
{
	Text_color(color); // 设定颜色
	Geometric_Coordinate(x1,x2,y1,y2); // 设定矩形坐标
	WriteCommand(0x90);
	if (fill)
	{
		WriteData(0xB0);
	}
	else
	{
		WriteData(0x90);
	}
	RA8875_WAITSTATUS();
}

/**************************************************************************************
函数功能：填充一个矩形区域
输入：    矩形的左上角和右下角坐标，和目标颜色
输出：	  无
**************************************************************************************/
/*void Lcd_cursorFillRec(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)//LcdG_FillRec
{
	RA8875_WAITSTATUS();
	Text_color(color); // 设定颜色
	Geometric_Coordinate(x1,x2,y1,y2-1); // 设定矩形坐标
	WriteCommand(0x90);
	WriteData(0xB0);
	RA8875_WAITSTATUS();
}*/


/**************************************************************************************
功能描述: 在屏幕显示一 16bit bmp图形
输    入: u16* image    	图形数据数组地址
          u16 x,y			左上角坐标
	  	  u16 width,height	图形的宽度和高度
输    出: 无
**************************************************************************************/
void LcdPrint16bitBmp(u16* image,u16 x,u16 y,u16 widht,u16 height)
{
	u16 w,h;
Memory_Write_LeftRight_TopDown();
	for(h=0;h<height;h++)
	{
		XY_Coordinate(x,y+h);
		WriteCommand(0x02);		//MRWC  REG[02h] -- Memory Read/Write Command
		for(w = 0; w< widht; w++)
		{
			WriteDataHS(*image++);
		//	Delay(20);
		}
	}
}

void LcdPrint8bitBmp(const u8* image,u16 x,u16 y,u16 widht,u16 height)
{
	//u8  color8;
	u16 w,h;
	//u16 r,g,b;
    
	for(h=0;h<height;h++)
	{
		XY_Coordinate(x,y+h);
		WriteCommand(0x02);		//MRWC  REG[02h] -- Memory Read/Write Command
		for(w = 0; w< widht; w++)
		{
			WriteData(*image++);
			
			//color8 = *image++;
			/*
			0123456700000000 
			67000 345000 01200
            0011 1000 1110 0011
	
			*/
			//r = (color8 & 0xE0)<<13; 
			//g = (color8 & 0x1C)<<6;
    		//b = (color8 & 0x03)<<3;
            
            //r = ((color8 & 0xE0)<<13)|0x1800; 
    		//g = ((color8 & 0x1C)<<6) |0xE0;
    		//b = ((color8 & 0x03)<<3) |0x07;
    		
            //r = (color8 & 0x03)<<14;
            //g = (color8 & 0x1C)<<6;
    		//b = (color8 & 0xE0)>>11; 
			
            //WriteData(r|g|b|0x38E3);
		}
	}
}
////////////////////////////////////////////////////////////////
void paint(u16 ColorBegin,u16 ColorEnd)
{
u8 r0 = (ColorBegin >> 11) & 0x1f;
u8 r1 = (ColorEnd >> 11) & 0x1f;
u8 g0 = (ColorBegin >> 5) & 0x1f;
u8 g1 = (ColorEnd >> 5) & 0x1f;
u8 b0 = ColorBegin & 0x1f;
u8 b1 = ColorEnd & 0x1f;
u16 i,F,rr,gg,bb,rgb;
for (i = 0; i < 400; i++)
{
F = (i << 16) / 400;
rr = r0 + ((F * (r1 - r0)) >> 16);
gg = g0 + ((F * (g1 - g0)) >> 16);
bb = b0 + ((F * (b1 - b0)) >> 16);
rgb = (rr << 11 | gg << 5 | bb);

LcdGeometryFillRec(i*1,0,(i+1)*1,49,1,rgb);

}
}
 

void BarGradient(void)
{
	u16 rr,gg,bb,i,rgb1,RStep,GStep,BStep,RLen,GLen,BLen,CurrentPix;
float RVarPerStep,GVarPerStep,BVarPerStep;

	RGBGradient.R = 0;
	RGBGradient.G = 0;
	RGBGradient.B = 0;	
	RGBGradient.RT = 1;
	RGBGradient.GT = 1;
	RGBGradient.BT = 0;	
	RGBGradient.RA = 1;
	RGBGradient.GA = 1;
	RGBGradient.BA = 1;
	RGBGradient.RStartPix = 0;
	RGBGradient.REndPix = 299;
	RGBGradient.GStartPix = 0;
	RGBGradient.GEndPix = 299;
	RGBGradient.BStartPix = 0;
	RGBGradient.BEndPix = 299;
	RGBGradient.BarLenth = 300;
	RGBGradient.BarStartPix=0;
	
	RStep = RGBGradient.REndPix - RGBGradient.RStartPix+1;
	RVarPerStep = (63-RGBGradient.R)/(RStep*1.0);
	
	GStep = RGBGradient.GEndPix - RGBGradient.GStartPix+1;
	GVarPerStep = (31-RGBGradient.G)/(GStep*1.0);
	
	BStep = RGBGradient.BEndPix - RGBGradient.BStartPix+1;
	BVarPerStep = (31-RGBGradient.B)/(BStep*1.0);
	
	for(i=0;i<RGBGradient.BarLenth;i++)
	{
		CurrentPix = RGBGradient.BarStartPix+i;
		if((CurrentPix>=RGBGradient.RStartPix)&&(CurrentPix<=RGBGradient.REndPix))
			if(RGBGradient.RT == 0)
				rr = RGBGradient.R + Myround((CurrentPix - RGBGradient.RStartPix) * RVarPerStep);
			else
				rr = 63- (RGBGradient.R + Myround((CurrentPix - RGBGradient.RStartPix) * RVarPerStep));
		else
			rr=0;
		
		
		if((CurrentPix>=RGBGradient.GStartPix)&&(CurrentPix<=RGBGradient.GEndPix))
		{
			if(RGBGradient.GT == 0)
				gg =  RGBGradient.R + Myround((CurrentPix - RGBGradient.GStartPix) * GVarPerStep);
			else
				gg =31-(RGBGradient.R + Myround((CurrentPix - RGBGradient.GStartPix) * GVarPerStep));
		}
		else
			gg=0;
		
		if(RGBGradient.BA == 0)
			bb=0;
		else
		{
			if((CurrentPix>=RGBGradient.BStartPix)&&(CurrentPix<=RGBGradient.BEndPix))
				bb = RGBGradient.B + Myround((CurrentPix - RGBGradient.BStartPix) * BVarPerStep);
			else
				bb=0;
		}
		
		
		rgb1 = (rr<<10)+(gg<<5)+(bb<<0);
		
		LcdGeometryFillRec(RGBGradient.BarStartPix+i*1,0,RGBGradient.BarStartPix+(i+1)*1,49,1,(rr<<10));
		LcdGeometryFillRec(RGBGradient.BarStartPix+i*1,50,RGBGradient.BarStartPix+(i+1)*1,99,1,(gg<<5));
		LcdGeometryFillRec(RGBGradient.BarStartPix+i*1,100,RGBGradient.BarStartPix+(i+1)*1,149,1,(bb<<0));
		LcdGeometryFillRec(RGBGradient.BarStartPix+i*1,150,RGBGradient.BarStartPix+(i+1)*1,199,1,rgb1);
	}

}
