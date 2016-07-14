/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * S5PC110 - LCD Driver for U-Boot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/types.h>
#include <s5pc11x.h>
#include <s5pc110.h>
#include <regs-fb.h>

#include "real210_lcd.h"

#define Inp32(_addr)		readl(_addr)
#define Outp32(addr, data)	(*(volatile u32 *)(addr) = (data))
#define Delay(_a)		udelay(_a*1000)

typedef struct {
	const char *name;	/* optional */
	u32 refresh;		/* optional */
	u32 xres;
	u32 yres;
	u32 pixclock;
	u32 left_margin;
	u32 right_margin;
	u32 upper_margin;
	u32 lower_margin;
	u32 hsync_len;
	u32 vsync_len;
	u32 sync;
	u32 vmode;
	u32 flag;
	char gdfBytesPP;
}fb_videomode;

fb_videomode lcd_timing = {

	.left_margin	= 46,
	.right_margin	= 210,
	.upper_margin	= 23,
	.lower_margin	= 22,
	.hsync_len	= 20,
	.vsync_len	= 13,
	.xres		= 800,
	.yres		= 480,
	.gdfBytesPP	= 3,
};
#ifdef  LCD_VIDEO_BACKGROUND
void lcd_win0_init()
{
	S5PC11X_FB * const fb = S5PC11X_GetBase_FB();
#if 0
	fb->VIDCON0 = ( S3C_VIDCON0_VIDOUT_RGB | S3C_VIDCON0_CLKVALUP_ALWAYS | S3C_VIDCON0_CLKVAL_F(1)|
	S3C_VIDCON0_VCLKEN_NORMAL | S3C_VIDCON0_CLKDIR_DIVIDED| S3C_VIDCON0_CLKSEL_HCLK |
	(0 << 18));
	
	/* RGB I/F控制信号。*/
	fb->VIDCON1 = ( S3C_VIDCON1_IVSYNC_INVERT | S3C_VIDCON1_IHSYNC_INVERT);
	
	/* 配置视频输出时序和显示尺寸。*/
	fb->VIDTCON0 = ( S3C_VIDTCON0_VBPD(lcd_timing.upper_margin) |
	S3C_VIDTCON0_VFPD(lcd_timing.lower_margin) |
	S3C_VIDTCON0_VSPW(lcd_timing.vsync_len));         
	fb->VIDTCON1 = ( S3C_VIDTCON1_HBPD(lcd_timing.left_margin) |
	S3C_VIDTCON1_HFPD(lcd_timing.right_margin) |
	S3C_VIDTCON1_HSPW(lcd_timing.hsync_len));
	fb->VIDTCON2 = ( S3C_VIDTCON2_LINEVAL(lcd_timing.yres - 1) |
	S3C_VIDTCON2_HOZVAL(lcd_timing.xres - 1));
#endif

	/* 窗口格式设置 */
	fb->WINCON0 = (S3C_WINCON_BPPMODE_24BPP_888 | S3C_WINCON_ENWIN_ENABLE |
	S3C_WINCON_WSWP_ENABLE);
	
	/* 指定OSD图像的左上角像素的横向屏幕坐标。*/
	fb->VIDOSD0A = ( S3C_VIDOSD_LEFT_X(0) | S3C_VIDOSD_TOP_Y(0));
	
	/* 指定横屏右下角的OSD图像的像素坐标。*/
	fb->VIDOSD0B = ( S3C_VIDOSD_RIGHT_X(lcd_timing.xres - 1) | S3C_VIDOSD_BOTTOM_Y(lcd_timing.yres - 1));
	/* 指定视频窗口0的大小控制寄存器。*/
	fb->VIDOSD0C = S3C_VIDOSD_SIZE(lcd_timing.xres * lcd_timing.yres);
	
	/* 指定视频窗口1的大小控制寄存器。*/
	//fb->VIDOSD1D = S3C_VIDOSD_SIZE(lcd_timing.yres * lcd_timing.yres);
	fb->SHADOWCON |= S3C_WINSHMAP_CH_ENABLE(0); //Enables Channel 0
	

	/* 清除显存 */
	memset((void *)LCD_VIDEO_BACKGROUND_ADDR, 0x00, VIDEO_MEM_SIZE);

	/* config Display framebuffer addr for console*/
	fb->VIDW00ADD0B0 = LCD_VIDEO_BACKGROUND_ADDR;
	/* This marks the end of the frame buffer. */
	fb->VIDW00ADD1B0 = (S3C_VIDW00ADD0B0 & 0xffffffff) + (lcd_timing.xres + 0) * lcd_timing.yres * 4;
	fb->VIDW00ADD2 = ((lcd_timing.xres * lcd_timing.gdfBytesPP) & 0x1fff);
	/* Enable  Display */
	//fb->VIDCON0 |= (S3C_VIDCON0_ENVID_ENABLE | S3C_VIDCON0_ENVID_F_ENABLE);  /* ENVID = 1  ENVID_F = 1*/
	//fb->TRIGCON = 3;//(TRGMODE_I80 | SWTRGCMD_I80);TRIGCON = 3
	
	//printf("\nVideo: video_hw_init complete \n\n");
}
#endif
void lcd_win1_init()
{
	S5PC11X_FB * const fb = S5PC11X_GetBase_FB();

	fb->VIDCON0 = ( S3C_VIDCON0_VIDOUT_RGB | S3C_VIDCON0_CLKVALUP_ALWAYS | S3C_VIDCON0_CLKVAL_F(3)|
	S3C_VIDCON0_VCLKEN_NORMAL | S3C_VIDCON0_CLKDIR_DIVIDED| S3C_VIDCON0_CLKSEL_HCLK |
	(0 << 18));
	
	/* RGB I/F控制信号。*/
	fb->VIDCON1 = ( S3C_VIDCON1_IVSYNC_INVERT | S3C_VIDCON1_IHSYNC_INVERT);
	
	/* 配置视频输出时序和显示尺寸。*/
	fb->VIDTCON0 = ( S3C_VIDTCON0_VBPD(lcd_timing.upper_margin) |
	S3C_VIDTCON0_VFPD(lcd_timing.lower_margin) |
	S3C_VIDTCON0_VSPW(lcd_timing.vsync_len));         
	fb->VIDTCON1 = ( S3C_VIDTCON1_HBPD(lcd_timing.left_margin) |
	S3C_VIDTCON1_HFPD(lcd_timing.right_margin) |
	S3C_VIDTCON1_HSPW(lcd_timing.hsync_len));
	fb->VIDTCON2 = ( S3C_VIDTCON2_LINEVAL(lcd_timing.yres - 1) |
	S3C_VIDTCON2_HOZVAL(lcd_timing.xres - 1));


	/* 窗口格式设置 */
	fb->WINCON1 = (S3C_WINCON_BPPMODE_24BPP_888 | S3C_WINCON_ENWIN_ENABLE |
	S3C_WINCON_WSWP_ENABLE);
	
	/* 指定OSD图像的左上角像素的横向屏幕坐标。*/
	fb->VIDOSD1A = ( S3C_VIDOSD_LEFT_X(0) | S3C_VIDOSD_TOP_Y(0));
	
	/* 指定横屏右下角的OSD图像的像素坐标。*/
	fb->VIDOSD1B = ( S3C_VIDOSD_RIGHT_X(lcd_timing.xres - 1) | S3C_VIDOSD_BOTTOM_Y(lcd_timing.yres - 1));

	fb->VIDOSD1C = ( S3C_VIDOSD_ALPHA0_R(LCD_VIDEO_BACKGROUND_ALPHA) |
		S3C_VIDOSD_ALPHA0_G(LCD_VIDEO_BACKGROUND_ALPHA) | S3C_VIDOSD_ALPHA0_B(LCD_VIDEO_BACKGROUND_ALPHA) );
	/* 指定视频窗口0的大小控制寄存器。*/
	fb->VIDOSD1D = S3C_VIDOSD_SIZE(lcd_timing.xres * lcd_timing.yres);
	
	/* 指定视频窗口1的大小控制寄存器。*/
	//fb->VIDOSD1D = S3C_VIDOSD_SIZE(lcd_timing.yres * lcd_timing.yres);
	fb->SHADOWCON = S3C_WINSHMAP_CH_ENABLE(1); //Enables Channel 1
	

	/* 清除显存 */
	memset((void *)LCD_VIDEO_ADDR, 0x00, VIDEO_MEM_SIZE);

	/* config Display framebuffer addr for console*/
	fb->VIDW01ADD0B0 = LCD_VIDEO_ADDR;
	/* This marks the end of the frame buffer. */
	fb->VIDW01ADD1B0 = (S3C_VIDW01ADD0B0 & 0xffffffff) + (lcd_timing.xres + 0) * lcd_timing.yres * 4;
	fb->VIDW01ADD2 = ((lcd_timing.xres * lcd_timing.gdfBytesPP) & 0x1fff);
	/* Enable  Display */
	fb->VIDCON0 |= (S3C_VIDCON0_ENVID_ENABLE | S3C_VIDCON0_ENVID_F_ENABLE);  /* ENVID = 1  ENVID_F = 1*/
	fb->TRIGCON = 3;//(TRGMODE_I80 | SWTRGCMD_I80);TRIGCON = 3
	
	//printf("\nVideo: video_hw_init complete \n\n");
}

void lcd_uninit()
{
	S5PC11X_FB * const fb = S5PC11X_GetBase_FB();

	/*关背光*/
	Outp32(GPD0CON, 0x0001);//GPD0CON GPD0CON[3]，[0]:output GPD0CON[2:1]:input  
	Outp32(GPD0PUD, 0x01);//GPD0PUD GPD0PUD[3]:Pull-up/ down disabled,GPD0PUD[2:0]:Pull-down enabled
	Outp32(GPD0DRV, 0x03);//GPD0DRV GPD0DRV[3:1]:4x,GPD0DRV[0]:4x
	Outp32(GPD0DAT, 0x00);//GPD0DAT GPD0_0 set 1

	/* 窗口0清除 */
	fb->VIDCON0 = 0x00000000;
	fb->WINCON0 = 0x00000000;		
			
	/* 窗口1清除 */
	fb->VIDCON1 = 0x00000000;
	fb->WINCON1 = 0x00000000;
	
	/* 通道使能清除 */
	fb->SHADOWCON = 0x00000000; 
		
	fb->TRIGCON = 0x00000000;
	memset((void *)LCD_VIDEO_ADDR, 0x00, VIDEO_MEM_SIZE);
}

void lcd_reset()
{
	S5PC11X_FB * const fb = S5PC11X_GetBase_FB();

	fb->WINCON1 &= ~(S3C_WINCON_BPPMODE_24BPP_888 | S3C_WINCON_ENWIN_ENABLE |
	S3C_WINCON_HAWSWP_ENABLE);
	lcd_uninit();
}

void lcd_clk_init() 
{
	//DISPLAY_CONTROL_REG = 0x2; //DISPLAY_CONTROL output path RGB=FIMD I80=FIMD ITU=FIMD
	*(volatile unsigned long *)(0xE0107008) = 0x2;
	
	//CLK_SRC1_REG = 0x700000;  //CLK_SRC1 fimdclk = EPLL
	*(volatile unsigned long *)(0xE0100204) = 0x700000;
} 

void lcd_Initialize(void)
{
	lcd_reset();
	/*开背光*/
	Outp32(GPD0CON, 0x0001);//GPD0CON GPD0CON[3]，[0]:output GPD0CON[2:1]:input  
	Outp32(GPD0PUD, 0x01);//GPD0PUD GPD0PUD[3]:Pull-up/ down disabled,GPD0PUD[2:0]:Pull-down enabled
	Outp32(GPD0DRV, 0x03);//GPD0DRV GPD0DRV[3:1]:4x,GPD0DRV[0]:4x
	Outp32(GPD0DAT, 0x01);//GPD0DAT GPD0_0 set 1	

	Outp32(GPF0CON, 0x22222222);	//set GPF0 as LVD_HSYNC,VSYNC,VCLK,VDEN,VD[3:0]
	Outp32(GPF0PUD,0x0);			//set pull-up,down disable
	Outp32(GPF1CON, 0x22222222);	//set GPF1 as VD[11:4]
	Outp32(GPF1PUD,0x0);			//set pull-up,down disable
	Outp32(GPF2CON, 0x22222222);	//set GPF2 as VD[19:12]
	Outp32(GPF1PUD,0x0);			//set pull-up,down disable
	Outp32(GPF3CON, 0x00002222);	//set GPF3 as VD[23:20]
	Outp32(GPF1PUD,0x0);			//set pull-up,down disable

	//--------- S5PC110 EVT0 needs MAX drive strength	---------//
	Outp32(GPF0DRV,0xffffffff);			//set GPF0 drive strength max by WJ.KIM(09.07.17)
	Outp32(GPF1DRV,0xffffffff);			//set GPF1 drive strength max by WJ.KIM(09.07.17)
	Outp32(GPF2DRV,0xffffffff);			//set GPF2 drive strength max by WJ.KIM(09.07.17)
	Outp32(GPF3DRV,0x3ff);					//set GPF3 drive strength max by WJ.KIM(09.07.17)
	
	lcd_win1_init();
#ifdef  LCD_VIDEO_BACKGROUND
	lcd_win0_init();
#endif
	lcd_clk_init();
	
	printf("LCD init complete!\n");
}

// 描点
void lcd_draw_pixel_win0(int x, int y, int color)
{
	unsigned long * pixel = (unsigned long  *)LCD_VIDEO_BACKGROUND_ADDR;

	*(pixel + y * LCD_WIDTH + x) = color;
}

// 清屏
void lcd_clear_screen_win0(int color)
{
	int i, j;

	for (i = 0; i < LCD_HEIGHT; i++)
		for (j = 0; j < LCD_WIDTH; j++)
			lcd_draw_pixel_win0(j, i, color);
}

// 描点
void lcd_draw_pixel_win1(int x, int y, int color)
{
	unsigned long * pixel = (unsigned long  *)LCD_VIDEO_ADDR;
	
	*(pixel + y * LCD_WIDTH + x) = color;
}

// 清屏
void lcd_clear_screen_win1(int color)
{
	int i, j;

	for (i = 0; i < LCD_HEIGHT; i++)
		for (j = 0; j < LCD_WIDTH; j++)
			lcd_draw_pixel_win1(j, i, color);
}

// 划横线
void lcd_draw_hline(int y, int x1, int x2, int color)
{
	int j;

	// 描第row行，第j列
	for (j = x1; j <= x2; j++)
		lcd_draw_pixel_win1(j, y, color);

}

// 划竖线
void lcd_draw_vline(int x, int y1, int y2, int color)
{
	int i;
	// 描第i行，第col列
	for (i = y1; i <= y2; i++)
		lcd_draw_pixel_win1(x, i, color);

}

void lcd_display_8_16(int x, int y, char *s, int word_color, int back_color,char back_color_flag)
{
	int i,j;
	u16 str;
	for(i = 0;i < 16;i ++)
	{
		str = *(s + i);
		for(j = 0;j < 8;j ++)
		{
			if(str & (0x80 >> j))
				lcd_draw_pixel_win1(x + j, y + i, word_color);
			else
			{
				if(back_color_flag)/*判断是否显示背景色*/
					lcd_draw_pixel_win1(x + j, y + i, back_color);
			}
		}	
	}
}

void lcd_display_16_16(int x, int y, char *s, int word_color, int back_color,char back_color_flag)
{
	int i,j;
	u16 str;
	for(i = 0;i < 16;i ++)
	{
		str = ((u16)(*(s + i * 2) << 8)) | (*(s + i * 2 + 1));
		for(j = 0;j < 16;j ++)
		{
			if(str & (0x8000 >> j))
				lcd_draw_pixel_win1(x + j, y + i, word_color);
			else
			{
				if(back_color_flag)/*判断是否显示背景色*/
					lcd_draw_pixel_win1(x + j, y + i, back_color);
			}
		}	
	}
}
void lcd_display_12_24(int x, int y, char *s, int word_color, int back_color,char back_color_flag)
{
	int i,j;
	u32 str;
	for(i = 0;i < 24;i ++)
	{
		str = (u32)((u32)(*(s + i * 2) << 8) | (u32)(*(s + i * 2 + 1)));/*两个字节表示一行*/
		for(j = 0;j < 12;j ++)
		{
			if(str & (0x8000 >> j))
				lcd_draw_pixel_win1(x + j, y + i, word_color);
			else
			{
				if(back_color_flag)/*判断是否显示背景色*/
					lcd_draw_pixel_win1(x + j, y + i, back_color);
			}
		}	
	}
}

void lcd_display_24_24(int x, int y, char *s, int word_color, int back_color,char back_color_flag)
{
	int i,j;
	u32 str;
	for(i = 0;i < 24;i ++)
	{
		str = (u32)(*(s + i * 3) << 16) | (u32)(*(s + i * 3 + 1) << 8) | (u32)(*(s + i * 3 + 2));
		for(j = 0;j < 24;j ++)
		{
			if(str & (0x800000 >> j))
				lcd_draw_pixel_win1(x + j, y + i, word_color);
			else
			{
				if(back_color_flag)/*判断是否显示背景色*/
					lcd_draw_pixel_win1(x + j, y + i, back_color);
			}
		}	
	}
}
void lcd_display_16_32(int x, int y, char *s, int word_color, int back_color,char back_color_flag)
{
	int i,j;
	u32 str;
	for(i = 0;i < 32;i ++)
	{
		str = (u32)((u32)(*(s + i * 2) << 8) | (u32)(*(s + i * 2 + 1)));/*两个字节表示一行*/
		for(j = 0;j < 16;j ++)
		{
			if(str & (0x8000 >> j))
				lcd_draw_pixel_win1(x + j, y + i, word_color);
			else
			{
				if(back_color_flag)/*判断是否显示背景色*/
					lcd_draw_pixel_win1(x + j, y + i, back_color);
			}
		}	
	}
}
void lcd_display_32_32(int x, int y, char *s, int word_color, int back_color,char back_color_flag)
{
	int i,j;
	u32 str;
	for(i = 0;i < 32;i ++)
	{
		str = ((u32)(*(s + i * 4) << 24)) | (u32)(*(s + i * 4 + 1) << 16) |
			(u32)(*(s + i * 4 + 2) << 8) | (u32)(*(s + i * 4 + 3));
		for(j = 0;j < 32;j ++)
		{
			if(str & (0x80000000 >> j))
				lcd_draw_pixel_win1(x + j, y + i, word_color);
			else
			{
				if(back_color_flag)/*判断是否显示背景色*/
					lcd_draw_pixel_win1(x + j, y + i, back_color);
			}
		}	
	}
}
