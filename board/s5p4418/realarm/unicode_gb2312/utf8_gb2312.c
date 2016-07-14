#include "utf8_gb2312.h"
#include "utf8_unicode.h"
#include "unicode_gb2312.h"

/***********************************************************************
*@函数名称: utf8_to_gb2312_one
*@功能描述: 把utf8格式的字符编码转换成gb2312格式的字符编码
*@参数:
	strfrom：	指向输入缓冲区, 以UTF-8编码 
	strto:		指向输出缓冲区, 其保存的数据即是转换完成的编码值 ，
			大小为2个字节
	
*@返回:		无
*@备注：	   	无
 **********************************************************************/
void utf8_to_gb2312_one(char* strfrom, char *strto)
{
	char strto_tmp[4];
	enc_utf8_to_unicode_one(strfrom, strto_tmp);
	cvt_Unicode2GB_one(strto_tmp, strto);
}
/***********************************************************************
*@函数名称: uitf8_to_gb2312_string
*@功能描述: 把utf8格式的字符串转换成ascii和gb2312格式的字符串
*@参数:	
	strfrom：	指向输入缓冲区, 以UTF-8编码 
	strto:		指向输出缓冲区, 其保存的数据即是转换完成的编码值 ，
			大小由strfrom决定
	
*@返回:		无
*@备注：	   本函数允许utf8格式字符串混合模式，即utf8即包含ascii也包含汉字编码的情况
 **********************************************************************/
void utf8_to_gb2312_string(char* strfrom, char *strto)
{
	char strto_tmp[4];
	int i = 0,j = 0;
	while(strfrom[i] != '\0')
	{
		if(strfrom[i] < 0xA0)
		{
			strto[j] = strfrom[i];/*如果是英文字符，那么不进行转换*/
			i += 1;
			j += 1;
		}
		else
		{
			utf8_to_gb2312_one(&strfrom[i], strto_tmp);
			strto[j] = strto_tmp[0];
			strto[j + 1] = strto_tmp[1];
			i += 3;//在linux下，汉字的编码是utf8编码，而utf8编码的汉字占用3个字节
			j += 2;//gb2312为2字节编码
		}
	}

	strto[j] = '\0';
}
