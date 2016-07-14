/**
 * A mini gui implementation (copied some code from VGUI)
 * functions:
 * 1. draw line (horz or vert only)
 * 2. draw rectangle, fill rectangle,
 * 3. draw bitmap ( Windows bmp format)
 * 4. draw text ( note, ascii only and the text font was gererated by
 * vgui's FontGen 1.0 tool)
 *
 * Usage:
 *  first maybe you want to change this line to fit your rgb format
	#define bmp_RGB_FORMAT bmp_RGB_FORMAT_565
 *  in the source code firstly you need to prepare all HW related jobs,
 *  such as turn on panel backlight/init LCDC
 *  after that the first function you need to invoked is bmp_initPrimary.
 *  You need to set the correct member in bmp_surface argument.
 *  After that you can call all functions
 *
 *  Here is a example:
 *
 *
	bmp_surface s;
        s.width = 800;
        s.height = 480;
	s.lineBytes = 800 * 2;
		// if the panel is 16bit
	s.startAddr = 0x7C000000;
	// the frame buffer address

	bmp_initPrimary(&s);

        bmp_Rect r;
	r.left = 0;
	r.right = s.width;
	r.top = 0;
	r.bottom = s.height;
	//fill whole screen with red color
	bmp_fillRect(&r, bmp_RGB2Color(255, 0, 0));
		
	//draw some text on the screen (build-in font size is 12x24)
	bmp_textOut(0, 0, "Hello,world!", bmp_RGB2Color(0, 0, 0));
	bmp_textOut(0, 25, "Hello, mini VGUI!", bmp_RGB2Color(0, 0, 0));
 *
 */
#ifndef MINIVGUI_H_INCLUDED
#define MINIVGUI_H_INCLUDED


#define bmp_RGB_FORMAT_555  555
#define bmp_RGB_FORMAT_565  565
#define bmp_RGB_FORMAT_888  888
#define bmp_RGB_FORMAT_666  666

//change this line if you are using other rgb format
#define bmp_RGB_FORMAT bmp_RGB_FORMAT_888

#if ( bmp_RGB_FORMAT == bmp_RGB_FORMAT_565)
	typedef unsigned short	bmp_color;
	#define bmp_RGB2Color(r, g, b)  (bmp_color)(((r) >> 3) << 11 | ((g) >> 2) << 5 | ((b) >> 3))

#elif ( bmp_RGB_FORMAT == bmp_RGB_FORMAT_555)
	typedef unsigned short	bmp_color;
	#define bmp_RGB2Color(r, g, b)  (bmp_color)(((r) >> 3) << 10 | ((g) >> 3) << 5 | ((b) >> 3) & 0x7fff)

#elif ( bmp_RGB_FORMAT == bmp_RGB_FORMAT_888)
	typedef unsigned int bmp_color;
	#define bmp_RGB2Color(r, g, b)	(bmp_color)(((r) << 16 | (g) << 8 | (b)) & 0x00FFFFFF)

#elif ( bmp_RGB_FORMAT == bmp_RGB_FORMAT_666)
	typedef unsigned int bmp_color;
	#define bmp_RGB2Color(r, g, b)  (bmp_color)(((r) >> 2) << 12 | ((g) >> 2) << 6 | ((b) >> 2))

#else
	#error "unknown rgb format."
#endif


typedef struct
{
	int     width;		/*图片宽度*/
	int     height;		/*图片高度*/
	int	start_x;	/*图片显示的起始x地址*/
	int	start_y;	/*图片显示的起始y地址*/
	int     lineBytes;	/*要显示图片每行的字节数，也就是图片的宽度*/
	int 	loadaddr;	/*BMP格式原图片数据在内存中的基地址*/
	int     dateaddr;	/*BMP图片解码后数据在内存中的基地址*/
}bmp_surface;

typedef struct
{
	int	 left;
	int      top;
	int      right;
	int      bottom;
}bmp_Rect;

// bmpData is the start address of a Win32 bmp file(support 32/24/16/8bit format)
// usually it is loaded from nand flash
// img->startAddr must be a valid memory address and can hold the image data
// return 0 if ok.
extern int bmp_loadBmp(const void* bmpData, bmp_surface* img);
//by kreal

#define DFRAMEBUFFER    IMAGE_EBOOT_FRAMEBUFFER_PA_START
#define LOGO_DATA_BUF   (EBOOT_USB_BUFFER_UA_START+0x400000)



#define EN_VERSION         1               //can set 0 to disable LABEL    
#define EN_PROCESS_BAR    1               //can set 0 to disable processbar
#define EN_LOGO           1

#define EBOOT_VERSION      "BOOT: 6.2.2"
#define NK_VERSION         "CORE :6.2.3"


#define DEFAULT_BACK_COLOR  bmp_RGB2Color(255, 0, 0)
#define DEFAULT_CHAR_COLOR  bmp_RGB2Color(255, 255, 255)
#define DEFAULT_BAR_COLOR   bmp_RGB2Color(00, 00, 255)



//note position
#define NOTE_POS_X  20
#define NOTE_POS_Y  0 

//processbar 
#define BAR_X_LEFT_RATE   1/4
#define BAR_X_RIGHT_RATE  3/4
#define BAR_Y_TOP_RATE    3/4 
#define BAR_HIGH          16
#define BAR_PRCOESS_NUM   40

//logo
#define LOGO_POS_X         0
#define LOGO_POS_Y         0

//lcd paramater translate
#define LCD_PARAM_ADDR1  0x85EFFFF0
#define LCD_PARAM_ADDR2  0x85EFFFF4
#define LCD_PARAM_ADDR3  0x85EFFFF8

#endif // MINIVGUI_H_INCLUDED


