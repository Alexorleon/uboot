#include <common.h>
#include <command.h>
#include "real210_key.h"

//#define KEY_GPIO_BASE	0xc001c000 	//该地址查手册得知
#define KEY_UP_GPIO_BASE	(PAD_GPIO_C + 2)
#define KEY_DOWN_GPIO_BASE	(PAD_GPIO_C + 8)
//volatile unsigned long *KEY_GPIO_OUT 	= KEY_GPIO_BASE;
//volatile unsigned long *KEY_GPIO_OUTENB = KEY_GPIO_BASE + 4;
//volatile unsigned long *KEY_GPIO_PUD = KEY_GPIO_BASE + 8;
extern int gpio_direction_input(unsigned gpio);
extern int gpio_get_value(unsigned gpio);
/***********************************************************************
*@函数名称: key_loop_conf
*@功能描述: 按键配置   
*@参数: 	无
*@返回:		无
*@备注：		外部调用
 **********************************************************************/
int key_loop_conf(void)
{
	/*配置GPIOC2、GPIOC8为输入引脚*/
	nxp_gpio_set_alt(KEY_UP_GPIO_BASE, 1);
	nxp_gpio_set_alt(KEY_DOWN_GPIO_BASE, 1);
	gpio_direction_input(KEY_UP_GPIO_BASE);
	gpio_direction_input(KEY_DOWN_GPIO_BASE);
	//printf("Key init cpmplete!\n");
	return 0;
}
/***********************************************************************
*@函数名称: key_loop_read
*@功能描述: 按键读取  
*@参数: 	key_vals  按键的结构体参数指针
*@返回:		1 有按键按下
		0 没有按键按下
*@备注：		无
 **********************************************************************/
static char key_read(real210_key_def *key_vals)
{
	unsigned long val;  	//用于读取按键端口数据
	real210_key_def key_tmp;
	/* 读取GPIOC2、GPIOB4按键值 对应按键是UP、DOWN */
	key_tmp.up = gpio_get_value(KEY_UP_GPIO_BASE);
	key_tmp.down = gpio_get_value(KEY_DOWN_GPIO_BASE);	
	if((key_tmp.up == 0) || (key_tmp.down == 0))
	{
		udelay(20 * 1000);/*做个延时之后重新读取键值判断，确保可靠性*/
		/*读取GPIOC2、GPIOB4按键值*/
		key_tmp.up = gpio_get_value(KEY_UP_GPIO_BASE);
		key_tmp.down = gpio_get_value(KEY_DOWN_GPIO_BASE);
		if((key_tmp.up == 0) || (key_tmp.down == 0))
		{
			if(!key_vals->flag)
			{
				key_vals->flag = 1;/*置位读取键值标志，目的是只成功读取一次，后面的等待释放时不再更新键值*/
				*key_vals = key_tmp;
			}
			return 1;
		}
		else
			return 0;		
	}
	else
		return 0;
	
}
/***********************************************************************
*@函数名称: key_read
*@功能描述: 按键循环读取，只有待按键释放之后才会执行返回
*@参数: 	key_vals  按键的结构体参数指针
*@返回:		1 有按键按下
		0 没有按键按下
*@备注：		外部调用
 **********************************************************************/
char key_loop_read(real210_key_def *key_vals)
{	
	key_vals->flag = 0;
	if(key_read(key_vals))
	{
		while(key_read(key_vals));/* 等待按键释放 */
		key_vals->flag = 0;/*复位读取键值标志，准备读取下一个键值*/
		return 1;
	}
	else
		return 0;
}
/***********************************************************************
*@函数名称: tst_key
*@功能描述: 按键测试，检测是否由按键按下   
*@参数: 	无
*@返回:		1 有按键按下
		0 没有按键按下
*@备注：		外部调用
 **********************************************************************/
char tst_key(void)
{
	real210_key_def *key_tmp = {0};
	if(key_read(key_tmp))
		return 1;
	else 
		return 0;
}
