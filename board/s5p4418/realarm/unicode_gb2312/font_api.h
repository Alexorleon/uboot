#ifndef FONT_API_H
#define FONT_API_H
#include <common.h> 

#define ASCII_8_16_Offset	0x000000
#define ASCII_12_24_Offset	0x000800
#define ASCII_16_32_Offset	0x002000
#define GB2312_16_16_Offset	0x004000
#define GB2312_24_24_Offset	0x043E40
#define GB2312_32_32_Offset	0x0D3A50
/*偏移地址是在制作的时候决定的*/

#define ASCII_8_16_AddressBase 		(GB2312_FONT_BASE_ADDR + ASCII_8_16_Offset)
#define ASCII_12_24_AddressBase		(GB2312_FONT_BASE_ADDR + ASCII_12_24_Offset)
#define ASCII_16_32_AddressBase		(GB2312_FONT_BASE_ADDR + ASCII_16_32_Offset)
#define GB2312_16_16_AddressBase	(GB2312_FONT_BASE_ADDR + GB2312_16_16_Offset)
#define GB2312_24_24_AddressBase	(GB2312_FONT_BASE_ADDR + GB2312_24_24_Offset)
#define GB2312_32_32_AddressBase	(GB2312_FONT_BASE_ADDR + GB2312_32_32_Offset)


#endif
