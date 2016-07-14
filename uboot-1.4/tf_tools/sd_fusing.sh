#
# Copyright (C) 2010 Samsung Electronics Co., Ltd.
#              http://www.samsung.com/
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
####################################
reader_type1="/dev/sdd"
#如果是使用的新卡（或者没有烧写过s5p4418程序的卡），那么需要把disk_tf_enable设置为1（默认值），执行完成后再设置为0,除非更换卡才再次设置为1,否则请保持为0,避免顺坏tf卡中原有的数据
disk_tf_enable=1


if [ -z $1 ]
then
    echo "usage: ./sd_fusing.sh <SD Reader's device file>"
    exit 0
fi

if [ $1 = $reader_type1 ]
then
	if [ $disk_tf_enable = 1 ]
	then
		partition1="$1"'1'
		partition2="$1"'2'
		partition3="$1"'3'
		partition4="$1"'4'
		

		if [ -b $1 ]
		then
			echo "$1 reader is identified."
		else
			echo "$1 is NOT identified."
			exit 0
		fi

		####################################
		# make partition
		echo "make sd card partition"
		echo "./sd_fdisk $1" 
		./sd_fdisk $1 
		dd iflag=dsync oflag=dsync if=sd_mbr.dat of=$1 
		rm sd_mbr.dat

		####################################
		# format
		umount $partition1 2> /dev/null
		umount $partition2 2> /dev/null
		umount $partition3 2> /dev/null
		umount $partition4 2> /dev/null

		echo "mkfs.vfat -F 32 $partition1"
		sudo mkfs.vfat -F 32 $partition1

		#echo "mkfs.ext2 $partition2"
		#mkfs.ext2 $partition2  

		#echo "mkfs.ext2 $partition3"
		#mkfs.ext2 $partition3  

		#echo "mkfs.ext2 $partition4"
		#mkfs.ext2 $partition4  
		#mount $partition1
	fi
else
	echo "Please check the TF card device number."
	exit 0
fi
####################################
#<BL1 fusing>
bl1_position=1
uboot_position=64

echo "2ndboot fusing"
dd iflag=dsync oflag=dsync if=2ndboot.bin of=$1 seek=$bl1_position


./u-boot-head-tool ../u-boot.bin #对u-boot进行加头处理

####################################
#<u-boot fusing>
echo "u-boot fusing"
dd iflag=dsync oflag=dsync if=u-boot-tmp.bin of=$1 seek=$uboot_position

rm u-boot-tmp.bin #删除u-boot-head-tool生成的临时文件
####################################
#<Message Display>
echo "U-boot image is fused successfully."
