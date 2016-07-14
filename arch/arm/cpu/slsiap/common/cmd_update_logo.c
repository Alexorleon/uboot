/*
 * (C) Copyright 2009
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <config.h>
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <mach-api.h>
#include <part.h>
#include <fat.h>
#include <fs.h>

#if (0)
#define	pr_dbg(msg...)	printf(msg)
#else
#define	pr_dbg(msg...)	do { } while (0)
#endif


extern int run_command (const char *cmd, int flag);

/*  priority
 *
 *	1. bootlogo environment
 *	2. loadbmp command parameters run_command("loadbmp \"ext4load mmc 1:1 0x43000000 logo.bmp;bootlogo 0x43000000\"", 0);
 *  3. macro command CONFIG_CMD_LOGO_LOAD
 */
static int do_logoupdate(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#define FILE_NAME_LOGO			"/sdfuse/logo.bmp"
#define FILE_NAME_ANDROID_LOGO	"/sdfuse/bootanimation.zip"
#define LOGO_DEV_PART		"0:1"
	unsigned long addr = SDFUSE_LOAD_ADDR;
	unsigned long bytes = 0;
	unsigned long pos = 0;
	int len_read, len_write;
	if(argc != 2)
	{
		printf("\nPlease input the right param!\n");
		printf("\nExamples,Input command\"logoupdate /sdfuse/logo.bmp\"\n");
		printf("*********************or \"logoupdate /sdfuse/bootanimation.zip\"\n\n");
		return -1;
	}
	if((!strcmp(argv[1],FILE_NAME_LOGO)) || (!strcmp(argv[1],FILE_NAME_ANDROID_LOGO)))
	{
		if(!strcmp(argv[1],FILE_NAME_LOGO))
	 	{
			//run_command("fatload mmc 2:1 48000000 /sdfuse/logo.bmp", 0);
			if(fs_set_blk_dev(SDFUSE_INTERFACE, SDFUSE_DEV_PART,FS_TYPE_FAT))/* 设置当前处于活动 */
				return -1;
			len_read = fs_read(FILE_NAME_LOGO, addr, pos, bytes);/* 读取数据 */
			printf("len_read is:%d\n", len_read);
		
			//if(fs_set_blk_dev(EMMC_DEV_NUM, LOGO_DEV_PART,FS_TYPE_FAT))/* 设置当前处于活动 */
			//	return -1;
			//run_command("mmc dev 0", 0);
			//len_write = ext4fs_write("/logo.bmp", addr, len_read);/* 读取数据 */
			//printf("len_write is:%d\n", len_write);
			run_command("ext4write mmc 0:1 48000000 /logo.bmp 1c2038", 0);
		}
		else if(!strcmp(argv[1],FILE_NAME_ANDROID_LOGO))
		{
			
		}
		
	}
	else
	{
		printf("\nTF card is not a logo.bmp or bootanimation.zip file.\n");
		printf("Plese check logo.bmp or bootanimation.zip file.\n");
	}
	return 0;
}

U_BOOT_CMD(
	logoupdate, 2, 1,	do_logoupdate,
	"update emmc bmpfile from TF card bmpfile ",
	"    - command: logoupdate /sdfuse/logo.bmp\n"
);

