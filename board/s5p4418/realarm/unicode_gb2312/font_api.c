#include <common.h> 
#include "font_api.h"
#include "bmp_lib.h"
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

int font_init()
{
	ExecuteCmd("mmc read 30f00000 5bba 1");/*读取字库存在标志*/
	ExecuteCmd("mw 31f00000 0xfafafafa 200");/*向内存中写入待比较的字库存在标志*/
	if(memcmp((void *)0x30f00000 , (void *)0x31f00000, 512))/*判断图片存在标志,memcmp相等返回0*/
	{
		/*读取fon字库文件到内存*/
		if(ExecuteCmd("fatload mmc 1:1 4f300000 /sdfuse/fontall.fon"))/*ExecuteCmd函数读取正确返回0*/
		{
			return -1;
		}
		else
		{
			ExecuteCmd("mmc write 31f00000 5bba 1");/*写入图片标志*/
			ExecuteCmd("mmc write 4f300000 5bbb 1000");/*把图片文件写入到emmc中*/	
		}
	}
	else
	{
		ExecuteCmd("mmc read 4f300000 5bbb 1000");/*读取字库*/
	}
	return 0;
}




