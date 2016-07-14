#ifndef REAL210_LCD_H
#define REAL210_LCD_H

#define LCD_WIDTH		800
#define LCD_HEIGHT		480

#define BACK_COLOR_ON		1
#define BACK_COLOR_OFF		0


void lcd_Initialize(void);
void lcd_draw_pixel_win0(int x, int y, int color);
void lcd_clear_screen_win0(int color);
void lcd_draw_pixel_win1(int x, int y, int color);
void lcd_clear_screen_win1(int color);
void lcd_draw_hline(int y, int x1, int x2, int color);
void lcd_draw_vline(int x, int y1, int y2, int color);

void lcd_display_8_16(int x, int y, char *s, int word_color, int back_color,char back_color_flag);
void lcd_display_16_16(int x, int y, char *s, int word_color, int back_color,char back_color_flag);
void lcd_display_12_24(int x, int y, char *s, int word_color, int back_color,char back_color_flag);
void lcd_display_24_24(int x, int y, char *s, int word_color, int back_color,char back_color_flag);
void lcd_display_16_32(int x, int y, char *s, int word_color, int back_color,char back_color_flag);
void lcd_display_32_32(int x, int y, char *s, int word_color, int back_color,char back_color_flag);

void lcd_display_string_16_16(int x, int y, char *s, int count, int word_color, int back_color,char back_color_flag);
void lcd_display_string_32_32(int x, int y, char *s, int count, int word_color, int back_color,char back_color_flag);

#endif
