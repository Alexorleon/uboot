#include <common.h>
#include <command.h>
#include <fdtdec.h>
#include <malloc.h>
#include <menu.h>
#include <post.h>
#include <version.h>
#include <watchdog.h>
#include <linux/ctype.h>
#include <asm/arch/fastboot.h>
#include "unicode_gb2312/ascii.h"
#include "key/real210_key.h"
/*#include "unicode_gb2312/display_api.h"
#include "unicode_gb2312/font_api.h"
*/
extern int run_command (const char *cmd, int flag);

DECLARE_GLOBAL_DATA_PTR;

#define DISPLAY_LEFT_X	10

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

#ifndef CONFIG_FASTBOOT_SDFUSE
/***********************************************************************
*@函数名称: sdfuse_uboot
*@功能描述: sdfuse烧写u-boot映像文件 
*@参数: 	无
*@返回:		无
*@备注：	   本u-boot未使用，real210.h中有CONFIG_FASTBOOT_SDFUSE这个定义
 **********************************************************************/
void sdfuse_uboot()
{
	char *buf;
	ulong checksum = 0;
	int i = 0;
	if(!ExecuteCmd("fatload mmc 1:1 30f00000 /sdfuse/u-boot.bin"))
	{
		buf = 0x30f00000 + 16;/*把u-boot首地址偏移16字节的内存地址赋值给buf，此时u-boot的首地址是30f00000*/
		for(i = 16;i < 8192;i++)/*循环进行u-boot的前8K代码校验和计算,i赋值为16是因为u-boot的前16字节可不必要校验和校验*/
		{
			checksum += *buf;
			buf++;		
		}
		*((volatile u32 *)(0x30f00000 + 0x8)) = checksum;/*把计算出的校验和写入3ff00008地址处等待写入EMMC*/
		printf("\nBL1 checksum is:%08x\n\n", checksum);
		
		/*printf("erase start block 1 (count 1073 blocks)...");
		ExecuteCmd("mmc erase 1 431");*/

		printf("writing BL1 start block 1 (count 16 blocks)...");
		ExecuteCmd("mmc write 30f00000 1 10");/*烧写BL1到eMMC的第一个block，长度为16 block（注意此时是10，但是为16进制）*/
		
		printf("\n");
		printf("writing u-boot.bin start block 49 (count 1024 blocks)...");
		ExecuteCmd("mmc write 30f00000 31 400");/*烧写u-boot到eMMC的第49个block，长度为1024 block（注意此时是400，但是为16进制）*/	
	}
}
/***********************************************************************
*@函数名称: sdfuse_kernel
*@功能描述: sdfuse烧写kernel映像文件 
*@参数: 	无
*@返回:		无
*@备注：	   本u-boot未使用，real210.h中有CONFIG_FASTBOOT_SDFUSE这个定义
 **********************************************************************/
void sdfuse_kernel()
{
	if(!ExecuteCmd("fatload mmc 1:1 30f00000 /sdfuse/uImage"))
		ExecuteCmd("mmc write 30f00000 800 2800");	
}
/***********************************************************************
*@函数名称: sdfuse_system
*@功能描述: sdfuse烧写system映像文件 
*@参数: 	无
*@返回:		无
*@备注：	   本u-boot未使用，real210.h中有CONFIG_FASTBOOT_SDFUSE这个定义
 **********************************************************************/
void sdfuse_system()
{
	if(!ExecuteCmd("fatload mmc 1:1 30f00000 /sdfuse/system.img"))
		ExecuteCmd("mmc write 30f00000 7788 23000");/*7D000是250MB，2300是70MB*/	
}
#endif
void cache_on()
{
	//ExecuteCmd("icache on");/*开启指令高速缓存*/
	ExecuteCmd("dcache on");/*开启数据高速缓存，提高下载和读取文件的速度*/	
}
void cache_off()
{
	//ExecuteCmd("icache off");/*开启指令高速缓存*/
	ExecuteCmd("dcache off");/*烧写完成关闭数据高速缓存*/
}
/***********************************************************************
*@函数名称: realarm_sdfuse
*@功能描述: sdfuse菜单显示和功能选择 
*@参数: 	无
*@返回:		无
*@备注：	   	无
 **********************************************************************/
void realarm_sdfuse(void)
{
	unsigned char select;
	while(1)
	{
		printf("\n#**** Select the fuction ****#\n");
		printf("[m] EMMC delect partition \n");	
		printf("[1] Flash all image\n");		
		printf("[2] Flash u-boot\n");			
		printf("[3] Flash boot\n");			
		printf("[4] Flash system\n");		
		//printf("[5] Flash userdata\n");			
		printf("[5] Exit\n");				
		printf("Enter your Selection:");		
	
		select = getc();
		printf("%c\n", select >= ' ' && select <= 127 ? select : ' ');	
		switch(select) 
		{
			case 'm':
				ExecuteCmd("mmc erase 0 800");

				break;
			case '1':
				ExecuteCmd("sdfuse flash all");

				break;
			
			case '2':
				ExecuteCmd("sdfuse flash 2ndboot");
				ExecuteCmd("sdfuse flash u-boot");

				break;
					
			case '3':
				ExecuteCmd("sdfuse flash boot");

				break;
			case '4':
				ExecuteCmd("sdfuse flash system");

				break;

			/*case '5':
				ExecuteCmd("sdfuse flash userdata");

				break;*/
			case '5':
				return;
			
			default:
				break;
		}
	}
}
/***********************************************************************
*@函数名称: tftp_burn
*@功能描述: tftp功能的具体实现
*@参数: 
	cmd：	执行的命令
*@返回:		无
*@备注：	   	无
 **********************************************************************/
void tftp_burn(char *cmd)
{
	char *buf;
	ulong checksum = 0;
	int i = 0;
	if (!strcmp(cmd, "u-boot"))
	{
		ExecuteCmd("tftp 30f00000 /u-boot/u-boot.bin");/*先拷贝u-boot.bin到内存30f00000处*/
		buf = 0x30f00000 + 16;/*把u-boot首地址偏移16字节的内存地址赋值给buf，此时u-boot的首地址是30f00000*/
		for(i = 16;i < 8192;i++)/*循环进行u-boot的前8K代码校验和计算,i赋值为16是因为u-boot的前16字节可不必要校验和校验*/
		{
			checksum += *buf;
			buf++;		
		}
		*((volatile u32 *)(0x30f00000 + 0x8)) = checksum;/*把计算出的校验和写入3ff00008地址处等待写入EMMC*/
		printf("\nBL1 checksum is:%08x\n\n", checksum);
		
		/*printf("erase start block 1 (count 1073 blocks)...");
		ExecuteCmd("mmc erase 1 431");*/

		printf("writing BL1 start block 1 (count 16 blocks)...");
		ExecuteCmd("mmc write 30f00000 1 10");/*烧写BL1到eMMC的第一个block，长度为16 block（注意此时是10，但是为16进制）*/
		
		printf("\n");
		printf("writing u-boot.bin start block 49 (count 1024 blocks)...");
		ExecuteCmd("mmc write 30f00000 31 400");/*烧写u-boot到eMMC的第49个block，长度为1024 block（注意此时是400，但是为16进制）*/
	}
	else if(!strcmp(cmd, "kernel"))	
	{
		ExecuteCmd("tftp 30f00000 /210_new/uImage");
		printf("write kernel at 800,count 2800\n");
		ExecuteCmd("mmc write 30f00000 800 2800");	
	}
	else if(!strcmp(cmd, "uboot_SD"))	
	{
		/*char *buf;
		ulong checksum = 0;
		int i = 0;*/
		ExecuteCmd("tftp 30f00000 /u-boot/u-boot.bin");/*先拷贝u-boot.bin到内存30f00000处*/
		buf = 0x30f00000 + 16;/*把u-boot首地址偏移16字节的内存地址赋值给buf，此时u-boot的首地址是30f00000*/
		for(i = 16;i < 8192;i++)/*循环进行u-boot的前8K代码校验和计算,i赋值为16是因为u-boot的前16字节可不必要校验和校验*/
		{
			checksum += *buf;
			buf++;		
		}
		*((volatile u32 *)(0x30f00000 + 0x8)) = checksum;/*把计算出的校验和写入3ff00008地址处等待写入SD/TF*/
		printf("\nBL1 checksum is:%08x\n\n", checksum);
		
		ExecuteCmd("mmc dev 1");/* u-boot默认是SD0，而开发板SD/TF挂接在SD2中，所以这里需要切换mmc设备号 */
		printf("writing BL1 start block 1 (count 16 blocks)...");
		ExecuteCmd("mmc write 30f00000 1 10");/*烧写BL1到SD/TF的第一个block，长度为16 block（注意此时是10，但是为16进制）*/
		
		printf("\n");
		printf("writing u-boot.bin start block 49 (count 1024 blocks)...");
		ExecuteCmd("mmc write 30f00000 31 400");/*烧写u-boot到SD/TF的第49个block，长度为1024 block（注意此时是400，但是为16进制）*/	
	}
	else if(!strcmp(cmd, "debug_uboot_RAM"))	
	{
		ExecuteCmd("tftp 30f00000 /u-boot/u-boot.bin");
		printf("Run u-boot in 30f00000\n");
		ExecuteCmd("go 30f00000");	
	}
}
/***********************************************************************
*@函数名称: realarm_tftp
*@功能描述: tftp功能菜单
*@参数: 	无
*@返回:		无
*@备注：	   	无
 **********************************************************************/
void realarm_tftp()
{
	unsigned char select;
	while(1)
	{
		printf("\n#**** Select the fuction ****#\n");
		/*由于system过大，所以tftp下不能下载文件系统*/
		printf("[1] Tftp burn u-boot\n");
		printf("[2] Tftp burn kernel\n");
		printf("[3] Tftp u-boot at SD/TF\n");
		printf("[4] Tftp debug u-boot at RAM\n");		
		printf("[5] Exit\n");
		printf("Enter your Selection:");
	
		select = getc();
		printf("%c\n", select >= ' ' && select <= 127 ? select : ' ');	
	
		switch(select) 
		{
			case '1':
				tftp_burn("u-boot");
				break;
			
			case '2':
				tftp_burn("kernel");
				break;
					
			case '3':/*此选项功能需要屏蔽u-boot配置头文件中的#define CONFIG_SKIP_LOWLEVEL_INIT，且CONFIG_SYS_TEXT_BASE需要修
				   改为30f00000*/
				tftp_burn("uboot_SD");/*u-boot 调试命令，tftp下载u-boot然后使用go命令运行之*/
				break;
			case '4':/*此选项功能需要屏蔽u-boot配置头文件中的#define CONFIG_SKIP_LOWLEVEL_INIT，且CONFIG_SYS_TEXT_BASE需要修
				   改为30f00000*/
				tftp_burn("debug_uboot_RAM");/*u-boot 调试命令，tftp下载u-boot然后使用go命令运行之*/
				break;

			case '5':
				return;
			
			default:
				break;
		}
	}	
}
/***********************************************************************
*@函数名称: emmc_format
*@功能描述: 格式化emmc 
*@参数: 	无
*@返回:		无
*@备注：	   本u-boot未使用该函数
 **********************************************************************/
void emmc_format(void)
{
	unsigned char select;
	printf("\n\nEnter 'y' to ensure erase emmc\n\n\n");	
	select = getc();
	if((select == 'y') || (select == 'Y'))	{
		ExecuteCmd("ext2format mmc 0:1");
		ExecuteCmd("ext2format mmc 0:2");
		ExecuteCmd("ext2format mmc 0:3");
		printf("\n\nformat complete !!!\n\n");	
	}
	else{
		printf("\nformat abort !!!\n\n");	
	}
}
/***********************************************************************
*@函数名称: emmc_check_partinfo
*@功能描述: 检测emmc是否有分区，如果有就跳过分区，在下面的菜单中可选择m进行分区不强制，反之  
*@参数: 	无
*@返回:		无
*@备注：	  emmc_partition_check函数在cmd_sdfuse.c文件中定义，功能是读取分区信息并把
	  分区1的起始地址赋值给SYSTEM_BLOCK_START，供给update_from_sd函数使用升级系统。
	  但是有时候板子已经少些过其他系统，emmc中已经有分区，那么本函数就失去判断能力了，所以
	  在烧写时如果不确定当前的分区，那么在烧写前要执行m进行分区操作。
 **********************************************************************/
void emmc_check_partinfo()
{
	int ret;
	unsigned char select;
	/*检测emmc已经做过里分区，这里是起保险起见，如果未检测到分区信息，那么强制进行分区，有询问。如果不提示，分区是可选的*/
	//ret = emmc_partition_check();
	if(ret < 0)
	{
		while(1)
		{			
			printf("Please, run 'm' and make partitions to the emmc\n");
			select = getc();
			printf("%c\n", select >= ' ' && select <= 127 ? select : ' ');
			if(select == 'M' | select == 'm')
			{
				ExecuteCmd("fdisk -c 0");
				break;
			}
		}
			
	}
	/*检测emmc已经做过里分区，这里是起保险起见，如果未检测到分区信息，那么强制进行分区，有询问。如果不提示，分区是可选的*/	
}
/***********************************************************************
*@函数名称: uboot_env_config
*@功能描述: u-boot的环境变量配置，可用于android和linux两个系统的启动配置 
*@参数: 	无
*@返回:		无
*@备注：		无
 **********************************************************************/
void uboot_env_config(void)
{
	unsigned char select;
	unsigned char cmd[200];

	while(1)
	{
		printf("\n#**** Select the fuction ****#\n");

		printf("[a or A] Android environment config\n");
		printf("[l or L] Linux environment config\n");
		printf("[n or N] NFS environment config\n");	
		printf("[e or E] Exit\n");
		printf("Enter your Selection:");
	
		select = getc();
		printf("%c\n", select >= ' ' && select <= 127 ? select : ' ');	
	
		switch(select) 
		{
			case 'A':case 'a'://设置android的启动参数
				ExecuteCmd("setenv bootcmd \"ext4load mmc 0:1 0x48000000 uImage;ext4load \
				mmc 0:1 0x49000000 root.img.gz;bootm 0x48000000\"");
				ExecuteCmd("setenv bootargs");
				ExecuteCmd(CONFIG_ANDROID_PARTS);
				ExecuteCmd("saveenv");
				break;
			
			case 'L':case 'l'://设置Linux的启动参数
				ExecuteCmd("setenv bootcmd \"ext4load mmc 0:1 0x48000000 uImage;bootm 0x48000000\"");
				ExecuteCmd("setenv bootargs \"console=ttyAMA0,115200 noinitrd root=/dev/mmcblk0p2 \
				rootfstype=ext4 rw init=/linuxrc\"");
				ExecuteCmd(CONFIG_LINUX_PARTS);
				ExecuteCmd("saveenv");
				break;
					
			case 'N':case 'n'://设置Linux下使用NFS时的启动参数，NFS有助于内核的调试
				ExecuteCmd("setenv bootcmd \"ext4load mmc 0:1 0x48000000 uImage;bootm 0x48000000\"");
				ExecuteCmd("setenv bootargs \"console=ttyAMA0,115200 noinitrd root=/dev/nfs init=/linuxrc \
				nfsroot=192.168.1.55:/wsh_space/nfsboot/linux/system_210/system \
				 ip=192.168.1.21:192.168.1.55:192.168.1.1:255.255.255.0::eth0:on\"");
				ExecuteCmd(CONFIG_LINUX_PARTS);
				ExecuteCmd("saveenv");
				break;

			case 'E':case 'e':
				return;
			
			default:
				break;
		}
	}
}
/***********************************************************************
*@函数名称: sdfuse_dis_key
*@功能描述: LCD上左下角显示两行提示字符  
*@参数: 	无
*@返回:		无
*@备注：		无
 **********************************************************************/
void sdfuse_dis_key(void)
{
#define KEY_AREA_COLORS 	0xffffff
	lcd_fill_rectangle(0, 400, 360, 200, KEY_AREA_COLORS, 0);
	lcd_draw_string(0, 520, 1, 1, 0, "%s", "Press UP  key, Burn all file");
	lcd_draw_string(0, 550, 1, 1, 0, "%s", "Press DOWN  key, Reset system");
}
/***********************************************************************
*@函数名称: RealARMMenu
*@功能描述: realarm开发板特定u-boot烧录选择菜单   
*@参数: 	无
*@返回:		无
*@备注：		无
 **********************************************************************/
void RealARMMenu(void)
{
	real210_key_def real210_key = {1, 1, 1};
	unsigned char select;
	u32 tmp;
	char *cmdline = getenv("bootargs");
	char font_flag = 0;/*0 不存在字库文件，默认使用英文显示  1 使用字库文件中的点阵数据，支持中文显示*/
	fboot_lcd_start();
	fboot_lcd_part("burn system", "Ready...");
#ifdef CONFIG_KEY_BURN_SYSTEM	
	sdfuse_dis_key();
#endif
	while(1) {

		printf("\n");
		printf("#***************** RealARM User Menu for Real4418-V6.2 *****************#\n");	
		printf("#**                            For Linux                              **#\n\n");	
		
		printf("cmdline = %s\n\n",cmdline);
		
		printf("-----------------------------Select---------------------------------\n");
		printf("\n[UP]  Press UP  key, Burn all file\n");
		printf("[DOWN] Press DOWN key, Reset system\n\n");	
		printf("[s or S] Burn image from SD card\n");
		printf("[f or F] Use fastboot burn system\n");
		printf("[u or U] u-boot Environment config\n");
		printf("[t or T] Burn u-boot and kernel from tftp\n");							
		printf("[b or B] Boot the system\n");
		printf("[r or R] Reboot the u-boot\n");
		printf("[e or E] Exit to command line\n");								

		printf("--------------------------------------------------------------------\n");
		printf("Enter your Selection:");

		while(1)
		{
		#ifdef CONFIG_KEY_BURN_SYSTEM  
			if(key_loop_read(&real210_key))
			{
				if(real210_key.up == 0)/* 按键UP  */
				{
					ExecuteCmd("sdfuse flash all");
					ExecuteCmd("reset");
				}
				
				if(real210_key.down == 0)/* 按键DOWN */
				{
					mdelay(3000);/* 延时3秒重启 */
					ExecuteCmd("reset");
				}
				break;

			}
		#endif
			if (tstc())/*检测是否有串口数据*/
			{
				select = getc();/*获取串口的数据值*/
				break;
			}			
		}

		/*select = getc();*//*已经在上面检测里，这里不再进行串口数据检测了*/
		printf("%c\n", select >= ' ' && select <= 127 ? select : ' ');

		switch(select) 
		{						
			case 'S': case 's':
				realarm_sdfuse();

				break;
			case 'F': case 'f':
				ExecuteCmd("fastboot");

				break;
			case 'U': case 'u':
				uboot_env_config();
				
				break;
			case 'T': case 't':
				//realarm_tftp();
				break;	
		
			case 'B': case 'b':
				ExecuteCmd(CONFIG_BOOTCOMMAND);
				break;

			case 'R': case 'r':
				ExecuteCmd("reset");
				break;

			case 'E': case 'e':
				return;								
					
			default:
				break;
		}
	}
}

