#include <common.h> 
#include "bmp_lib.h"
#include "display_api.h"
#include "font_api.h"
#include "utf8_gb2312.h"

bmp_surface bmpimg;

extern int run_command (const char *cmd, int flag);
/***********************************************************************
*@函数名称: ExecuteCmd
*@功能描述: 执行cmd字符串命令
*@参数:
	cmd：	执行的命令
*@返回:		无
*@备注：	   	无
 **********************************************************************/
static int ExecuteCmd(char *cmd)
{
	return run_command(cmd, 0);
}
//load bmp 
int LoadBmp(int x, int y)
{
	u32 tmp;
	bmpimg.dateaddr =  BMP_DATE_ADDR;
	bmpimg.loadaddr =  BMP_LOAD_ADDR;
	bmpimg.width = LCD_WIDTH;
	bmpimg.height = LCD_HEIGHT;
	bmpimg.start_x = x;
	bmpimg.start_y = y;
	bmpimg.lineBytes = LCD_WIDTH;		

	ExecuteCmd("mmc read 30f00000 5000 1");/*读取图片存在标志*/
	tmp = *(volatile unsigned long *)0x30f00000;
	if(tmp == 0xfafafafa)/*判断图片存在标志*/
	{	
		ExecuteCmd("mmc read 30f00000 5001 bb8");/*图片如果存在，那么读取图片数据到指定内存位置*/
		if(!bmp_loadBmp((void*)bmpimg.loadaddr,&bmpimg))/*从TF卡读取图片文件,并判断读取是否成功*/
		{
			/*判断TF卡中的图片与emmc中的图片是否一样，比较100个4字节即可,如果为不相等，那么更新emmc中的图片数据*/
			if(memcmp((void *)0x30f00000 , (void *)0x31f00000, 100))
			{
				ExecuteCmd("mw 31effffc fafafafa");/*设定图片存在标志值*/

				ExecuteCmd("mmc write 31effffc 5000 1");/*写入图片标志*/
				ExecuteCmd("mmc write 31f00000 5001 bb8");/*把图片文件写入到emmc中*/
				memcpy((void *)0x30f00000, (void *)0x31f00000, VIDEO_MEM_SIZE);	
			}
			
		}	
	}
	else
	{		
		if(!bmp_loadBmp((void*)bmpimg.loadaddr,&bmpimg))/*从TF卡读取图片文件,并判断是否存在图片文件*/
		{
			ExecuteCmd("mw 31effffc fafafafa");/*设定图片存在标志值*/
			ExecuteCmd("mmc write 31effffc 5000 1");/*写入图片标志*/
			ExecuteCmd("mmc write 31f00000 5001 bb8");/*把图片文件写入到emmc中*/

			memcpy((void *)0x30f00000, (void *)0x31f00000, VIDEO_MEM_SIZE);/*完成图片烧写之后复制数据到30f00000地址*/
		}	
		else
			return -1;
	}

	memcpy((void *)(LCD_VIDEO_BACKGROUND_ADDR + (y * bmpimg.width + x) * 4), 
			(void *)(0x30f00000 + (y * bmpimg.width + x) * 4), 
			bmpimg.width * bmpimg.height * 4);/*搬移图片数据到背景窗口win0显存位置*/
	memcpy((void *)(LCD_VIDEO_ADDR + (y * bmpimg.width + x) * 4), 
			(void *)(LCD_VIDEO_BACKGROUND_ADDR + (y * bmpimg.width + x) * 4), 
			bmpimg.width * bmpimg.height * 4);/*复制背景窗口win0的显存数据到窗口1的显存位置*/
	return 0;
}
/***********************************************************************
*@函数名称: bmp_display_flush
*@功能描述: LCD显示图片刷新，
*@参数:
	x，y：	刷新的起始地址，大小为图片的大小
*@返回:		无
*@备注：	   	无
 **********************************************************************/
void bmp_display_flush(int x, int y)
{
	memcpy((void *)(LCD_VIDEO_ADDR + (y * bmpimg.width + x) * 4), 
		(void *)(LCD_VIDEO_BACKGROUND_ADDR + (y * bmpimg.width + x) * 4), 
		bmpimg.width * bmpimg.height * 4);
}

/*
函数功能：计算汉字点阵数据在W25X16芯片中的偏移首地址          
参数：
      Hz：汉字字符串的首地址，总共两个字节Hz[0]和Hz[1]
调用   :内部掉用
调用举例：Hz_Adress("您");或者Hz_Adress(Hz)；
*/
unsigned int get_address(const char *str, char display_type)
{

	unsigned int address;
	if(str[0] < 0xA0)//判断是汉字还是ASCII字符，ASCII字符范围是0~0x95，汉字是0xA1A0~0xF7FD
	/*计算ASCII点阵数据的首地址，之后为汉字的点阵数据
	*/
	{
		
		if(display_type == DISPLAY_16_16)
			address = str[0] * 16 + ASCII_8_16_AddressBase;
		else if(display_type == DISPLAY_24_24)
			address = str[0] * 48 + ASCII_12_24_AddressBase;
		else if(display_type == DISPLAY_32_32)
			address = str[0] * 64 + ASCII_16_32_AddressBase;
		else/*缺省为16*16点阵*/
			address = str[0] * 16 + ASCII_8_16_AddressBase;
	}
	else
	/*计算汉字点阵数据的首地址，即汉字在字库的首地址
	*/
	{
		
		if(display_type == DISPLAY_16_16)
			address = (94 * (str[0] - 0xa1) + (str[1] - 0xa0 - 1)) * 32 + GB2312_16_16_AddressBase;  
		else if(display_type == DISPLAY_24_24)
			address = (94 * (str[0] - 0xa1) + (str[1] - 0xa0 - 1)) * 72 + GB2312_24_24_AddressBase;  
		else if(display_type == DISPLAY_32_32)
			address = (94 * (str[0] - 0xa1) + (str[1] - 0xa0 - 1)) * 128 + GB2312_32_32_AddressBase;  
		else/*缺省为16*16点阵*/
			address = (94 * (str[0] - 0xa1) + (str[1] - 0xa0 - 1)) * 32 + GB2312_16_16_AddressBase;  
	}
	return address;
}
void display_GB2312_ASCII_one(char display_type, int x,int y,const char *str,
				int word_color, int back_color,char back_color_flag)
{
	unsigned int address;
	address = get_address(str, display_type);/*获取str（str汉字编码已从utf8转为了gb2312）字符在字库中点阵数据的首地址*/
	if(str[0] < 0xA0)//判断是汉字还是ASCII字符，ASCII字符范围是0~0x95，汉字是0xA1A0~0xF7FD &ascii_8_16[str[0] * 16]
	{
		if(display_type == DISPLAY_16_16)
			lcd_display_8_16(x, y, (char *)address, word_color, back_color, back_color_flag);
		else if(display_type == DISPLAY_24_24)
			lcd_display_12_24(x, y, (char *)address, word_color, back_color, back_color_flag);
		else if(display_type == DISPLAY_32_32)
			lcd_display_16_32(x, y, (char *)address, word_color, back_color, back_color_flag);
		else/*缺省为16*16点阵*/
			lcd_display_8_16(x, y, (char *)address, word_color, back_color, back_color_flag);	
	} 
	else
	{  /*读出数据，总共是32个字节*/
		
		if(display_type == DISPLAY_16_16)
			lcd_display_16_16(x, y, (char *)address, word_color, back_color, back_color_flag);
		else if(display_type == DISPLAY_24_24)
			lcd_display_24_24(x, y, (char *)address, word_color, back_color, back_color_flag);
		else if(display_type == DISPLAY_32_32)
			lcd_display_32_32(x, y, (char *)address, word_color, back_color, back_color_flag);
		else/*缺省为16*16点阵*/
			lcd_display_16_16(x, y, (char *)address, word_color, back_color, back_color_flag);
	}
}
void display_GB2312_ASCII_string(char display_type, int x,int y,char *str,
				int word_color, int back_color,char back_color_flag)
{
	int i = 0;
	char cvt_str[512];/*存放转换好的字符串数据*/
	memset(cvt_str, 0x00, 512);
	utf8_to_gb2312_string(str, cvt_str);/*把str中的汉字utf8编码转换为gb2312编码，英文字符不做处理，字符的顺序已做处理，不影响原字符顺序*/

	while(cvt_str[i] != '\0') 
  	{
	    	if(cvt_str[i] < 0xA0)//判断是汉字还是ASCII字符，ASCII字符范围是0~0x95，汉字是0xA1A0~0xF7FD,utf8汉字范围是大于0xe0
	    	{
			
			//显示一个ASCII字
	      		display_GB2312_ASCII_one(display_type, x, y, &cvt_str[i], word_color, back_color, back_color_flag);
			i = i + 1;  
      
			if(display_type == DISPLAY_16_16)
				x = x + 8;      //每显示完一个汉字，x列坐标后移一个汉字的宽度
			else if(display_type == DISPLAY_24_24)
				x = x + 12;
			else if(display_type == DISPLAY_32_32)
				x = x + 16;
			else/*缺省为16*16点阵*/
				x = x + 8;      //每显示完一个汉字，x列坐标后移一个汉字的宽度
	    	}
	    	else
	    	{
			//显示一个汉字
	      		display_GB2312_ASCII_one(display_type, x, y, &cvt_str[i], word_color, back_color, back_color_flag);

	      		i = i + 2;    

			if(display_type == DISPLAY_16_16)
				x = x + 16;      //每显示完一个汉字，x列坐标后移一个汉字的宽度
			else if(display_type == DISPLAY_24_24)
				x = x + 24;
			else if(display_type == DISPLAY_32_32)
				x = x + 32;
			else/*缺省为16*16点阵*/
				x = x + 16;      //每显示完一个汉字，x列坐标后移一个汉字的宽度

	    	}
  	}
}
void display_ASCII_one(int x,int y,const char *str, int word_color, int back_color,char back_color_flag)
{
	unsigned int address;
	address = get_address(str, 0);
	lcd_display_8_16(x, y, (char *)address, word_color, back_color, back_color_flag);
}
void display_ASCII_string(int x,int y,char *str, int word_color, int back_color,char back_color_flag)
{
	int i = 0;
	while(str[i] != '\0') 
	{
		display_ASCII_one(x, y, &str[i], word_color, back_color, back_color_flag);
		i = i + 1; 
		x += 8;
	}
}

void display_clear(int color)
{
	lcd_clear_screen_win1(color);
}
void display_string_16_16(int x, int y, char *s, int count, int word_color, int back_color,char back_color_flag)
{
	int c;
	for(c = 0;c < count;c ++)
	{
		lcd_display_16_16(x + c * 16, y, s + c * 32, word_color, back_color, back_color_flag);
	}	
}
void display_string_24_24(int x, int y, char *s, int count, int word_color, int back_color,char back_color_flag)
{
	int c;
	for(c = 0;c < count;c ++)
	{
		lcd_display_24_24(x + c * 24, y, s + c * 72, word_color, back_color, back_color_flag);
	}	
}
void display_string_32_32(int x, int y, char *s, int count, int word_color, int back_color,char back_color_flag)
{
	int c;
	for(c = 0;c < count;c ++)
	{
		lcd_display_32_32(x + c * 32, y, s + c * 128, word_color, back_color, back_color_flag);
	}	
}
