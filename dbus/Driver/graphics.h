
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

//-------------------------------------------------------------------------------------
#include <stm32f4xx.h>

#define   RED				0xF800      	//红色
#define   GREEN			0x07E0				//绿色
#define   BLUE			0x001F      	//蓝色
#define   CYAN			0x07FF      	//青色
#define   YELLOW		0xFFE0      	//黄色
#define   YH				0xF81F      	//洋红
#define   WHITE			0xFFFF		//白色   
#define   BLACK    0x0000      	//黑色

#define   GRAY0    0xCE9A   	//灰色0 3165 00110 001011 00101
#define   GRAY1    0x8410      	//灰色1      00000 000000 00000
#define   GRAY2    0x4208      	//灰色2  1111111111011111
#define   BLUE0     0xEE56      //浅蓝色\

#define   STARTADDR     200      //浅蓝色

extern void LcdClear(u16 color);
extern void LcdPrintchar(char ch,u16 x,u16 y,u16 color,u16 bcolor,u8 mode);
extern void LcdPrintHz(u8 *font,u16 x,u16 y,u16 color,u16 bcolor,u8 mode);
extern void LcdPrintStr(char *str,u16 x,u16 y,u16 color,u16 bcolor,u8 mode);
extern void LcdPrintStrMid(u8 *str,u8 x,u16 y,u8 pixlen);
extern void LcdPrintf(u16 x,u16 y,u16 color,u16 bcolor,u8 mode,char *fmt,...);
extern void LcdSetArea(u16 x1, u16 y1, u16 x2, u16 y2);
extern void LcdPrintDot(u16 x, u16 y, u16 color);
extern void LcdGeometryHorz(u16 x, u16 y, u16 width, u16 color);
extern void LcdGeometryHorzD(u16 x, u16 y, u16 width, u16 color);
extern void LcdGeometryVertical(u16 x, u16 y, u16 height, u16 color);
extern void LcdGeometryBIAS(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
extern void LcdGeometryFillRec(u16 x1, u16 y1, u16 x2, u16 y2, u8 fill, u16 color);
extern void LcdPrint16bitBmp(u16* image,u16 x,u16 y,u16 widht,u16 height);
extern void LcdPrint8bitBmp(const u8* image,u16 x,u16 y,u16 widht,u16 height);
//extern void LcdPrintVertZuHe(u16 x, u16 y, u16 height, u16 y1);
extern void LcdPrintHorzZuHe(u16 x, u16 y, u16 width, u16 color);
void LcdPrintBIAS(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void Lcd_cursorFillRec(u16 x1, u16 y1, u16 x2, u16 y2,u16 color);
#endif
