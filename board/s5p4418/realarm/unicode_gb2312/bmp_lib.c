#include <common.h>
#include <hush.h>
#include "bmp_lib.h"
#include "real210_lcd.h"
#include "display_api.h"
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
/***********************************************************************
*@函数名称: bmp_draw_set
*@功能描述: 向bmp图片解压数据的内存区域写入图片数据
*@参数:
	s：	bmp图片的相关信息结构体
	x,y:	像素点的xy坐标
	color：	当前像素点的颜色值
*@返回:		无
*@备注：	   	无
 **********************************************************************/
int bmp_draw_set(bmp_surface * s,int x, int y,int color)
{
	unsigned long * pixel = (unsigned long  *)s->dateaddr;
    	*(pixel + x + s->lineBytes * y) = color;
}
/***********************************************************************
*@函数名称: bmp_loadBmp
*@功能描述: 从TF中载入bmp图片
*@参数:
	bmpData：	bmp图片数据流首地址
	img：		bmp图片的相关信息结构体
*@返回:		无
*@备注：	   	无
***********************************************************************/
int bmp_loadBmp(const void* bmpData, bmp_surface* img)
{
	unsigned char r,g,b;
	int i, j, bytes;
	int x,y;
	int width, height, offset;
	short colorBits;
	unsigned char *colorMap;

	unsigned char* fileBuffer = ( unsigned char*)bmpData;
	if(ExecuteCmd("fatload mmc 1:1 4f600000 /sdfuse/logo.bmp"))
		return -1;/*读取bmp图片到内存*/

	if (*fileBuffer != 'B')
	return -1;


	//offset = *(int*)(fileBuffer + 10); //Æ«ÒÆÁ¿, 4Byte
	//width = *(int*)(fileBuffer + 18);   //¿í¶È, 4Byte
	//height = *(int*)(fileBuffer + 22);  //žß¶È, 4Byte

	offset = (fileBuffer[13]<<24)|(fileBuffer[12]<<16)|(fileBuffer[11]<<8)|(fileBuffer[10]<<0);
	width = (fileBuffer[21]<<24)|(fileBuffer[20]<<16)|(fileBuffer[19]<<8)|(fileBuffer[18]<<0);
	height = (fileBuffer[25]<<24)|(fileBuffer[24]<<16)|(fileBuffer[23]<<8)|(fileBuffer[22]<<0);
	// colorBits =  *(short*)(fileBuffer + 28);   //É«Éî, 2Byte

	colorBits = (fileBuffer[29]<<8)|(fileBuffer[28]<<0);

	colorMap   = (unsigned char*)(fileBuffer + 54);  //ÑÕÉ«±íµØÖ·(8bitÊ±Ê¹ÓÃ)

	switch (colorBits) {
	case 32:     // r,g,b,a
		for (i = height - 1; i >= 0; i--) {
		    for (j = 0; j < width; j++) {
			b = *(fileBuffer + offset);
			g = *(fileBuffer + offset + 1);
			r = *(fileBuffer + offset + 2);
			bmp_draw_set(img, j, i, bmp_RGB2Color(r,g,b));
			offset += 4;
		    }
		}
		memcpy((void *)0x31f00000, (void *)img->dateaddr, VIDEO_MEM_SIZE);
		break;

	case 24:     // r,g,b
		bytes = 4 - (width * 3) % 4;
		if (bytes == 4)
		    bytes = 0;
		for (y = height-1; y >= 0; y--) {
		    for (x = 0; x < width; x++) {
			b = *(fileBuffer + offset);
			g = *(fileBuffer + offset + 1);
			r = *(fileBuffer + offset + 2);
			bmp_draw_set(img, x, y, bmp_RGB2Color(r,g,b));
			offset += 3;
		    }
		    offset  += bytes;
		}
		memcpy((void *)0x31f00000, (void *)img->dateaddr, VIDEO_MEM_SIZE);
		break;
	case 16:      //16bit 555
		{
		    unsigned short color16;

		    bytes = 4 - (width * 2) % 4;
		    if (bytes == 4)
			bytes = 0;
		    for (i = height - 1; i >= 0; i--) {
			for (j = 0; j < width; j++) {
			    color16 = *(unsigned short *)(fileBuffer+offset);
#if ( GUI_RGB_FORMAT == RGB_FORMAT_565)
		   // bmp_draw_set(img, j, i) = (color16<<1) & 0xFFC0 | (color16  & 0x1F);
#else
		   // bmp_draw_set(img, j, i) = bmp_RGB2Color((color16 & 0x7C00) >> 7, (color16 & 0x03E0) >> 2, (color16 & 0x001F) << 3);
#endif
			    offset += 2;
			}
			offset  += bytes;
		    }
		}
		memcpy((void *)0x31f00000, (void *)BMP_DATE_ADDR, VIDEO_MEM_SIZE);
		break;
	case 8:     //256 colormap
		bytes = 4 - width % 4;
		if (bytes == 4)
		    bytes = 0;
		for (i = height - 1; i >= 0; i--) {
		    for (j = 0; j < width; j++) {
			int offSet = *(fileBuffer + offset) * 4;
			b = colorMap[offSet];
			g = colorMap[offSet + 1];
			r = colorMap[offSet + 2];
			//bmp_draw_set(img, j, i) = bmp_RGB2Color(r,g,b);
			offset ++;
		    }
		    offset  += bytes;
		}
		memcpy((void *)0x31f00000, (void *)img->dateaddr, VIDEO_MEM_SIZE);
		break;
	default:
		break;
	}
   	
    	return 0;
}

