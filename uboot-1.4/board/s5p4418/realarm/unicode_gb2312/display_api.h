#ifndef DISPLAY_API_H
#define DISPLAY_API_H

#include "real210_lcd.h"

#define BMP_LOAD_ADDR			(0x4f600000)/*BMP图片存放的内存基址*/
#define BMP_DATE_ADDR			(0x4f800000)/*BMP图片解码后存放的内存基址*/

#define DISPLAY_16_16			0
#define DISPLAY_24_24			1
#define DISPLAY_32_32			2

int LoadBmp(int x, int y);
void bmp_display_flush(int x, int y);
void display_GB2312_ASCII_string(char display_type, int x,int y,char *str,
				int word_color, int back_color,char back_color_flag);
void display_clear(int color);
void display_string_16_16(int x, int y, char *s, int count, int word_color, int back_color,char back_color_flag);
void display_string_24_24(int x, int y, char *s, int count, int word_color, int back_color,char back_color_flag);
void display_string_32_32(int x, int y, char *s, int count, int word_color, int back_color,char back_color_flag);

void display_ASCII_string(int x,int y,char *str, int word_color, int back_color,char back_color_flag);
#endif
