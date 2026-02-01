
// ****************************************************************************
//
//                   HDMI and VGA display driver for Vzlet Z256
//                                Setup videomode
//
// ****************************************************************************

#include "../include.h"

ALIGNED u8 FrameBuf[FRAMESIZE]; // pointer to display frame buffer
int Vmode = VMODE_OFF;	// current videomode VMODE_*
int Width;	// current videomode width in pixels
int WidthByte;	// current videomode length of line in bytes
int Height;	// current videomode height
int Hdbl;	// horizontal divide
int Vdbl;	// vertical divide
int VideoFormat; // video format
u16* VideoPal;	// video palettes
sDispHstxVModeState VModeState[2]; // videomode descriptors
int VModeStateInx = 0; // index of current videomode descriptor

// 640x600/60.3Hz/37.9kHz, pixel clock 32 MHz, system clock 320 MHz ... detected as 800x600@60Hz on VGA
const sDispHstxVModeTime VModeTime = {
	True,		// u8	hpol;	// HSYNC polarity 0=negative, 1=positive
	True,		// u8	vpol;	// VSYNC polarity 0=negative, 1=positive
	2,		// u8	clkdiv;	// system clock divider (1 or 2)
	0,		// u8	res;	// ... reserved (align)

	// horizontal timings
	640,		// u16	hactive;// horizontal active pixels (must be even number)
	32,		// u16	hfront;	// horizontal front porch in pixels (must be even number)
	102,		// u16	hsync;	// HSYNC width in pixels (must be even number)
	70,		// u16	hback;	// horizontal back porch in pixels (must be even number)
	844,		// u16	htotal;	// horizontal total width in pixels

	// vertical timings
	600,		// u16	vactive;// vertical active scanlines
	1,		// u16	vfront;	// vertical front porch in scanlines
	4,		// u16	vsync;	// VSYNC height in scanlines
	23,		// u16	vback;	// vertical back porch in scanlines
	628,		// u16	vtotal;	// vertical total height in scanlines

	// clock (shifter: 5 clock cycles per pixel, expander: 1 loop per shifter, divider: 1 or 2)
	320000,		// u32	sysclk;	// system clock in kHz (= 5x or 10x video pixel clock)
};

// default color 2-bit palettes (do not use "const" - should be in RAM)
u16 DispHstxPal2bcol1[4] ALIGNED = { COL16_BLACK, COL16_BLUE, COL16_RED, COL16_WHITE, };
u16 DispHstxPal2bcol2[4] ALIGNED = { COL16_BLACK, COL16_GREEN, COL16_RED, COL16_YELLOW, };

// prepare videomode parameters
void PrepVideomode(int videomode)
{
	// save current videomode
	Vmode = videomode;

	// prepare parameters (native videomode is 640x600)
	switch (videomode)
	{
	// 640x600, 1 bit per pixel, B&W levels
	case VMODE_640x600x1:
		Width = 640;
		WidthByte = 640/8;
		Height = 600;
		Hdbl = 1;
		Vdbl = 1;
		VideoFormat = DISPHSTX_FORMAT_1_PAL;
		VideoPal = DispHstxPal1b;
		break;

	// 640x300, 2 bits per pixel, 4 gray levels
	case VMODE_640x300x2bw:
		Width = 640;
		WidthByte = 640/4;
		Height = 300;
		Hdbl = 1;
		Vdbl = 2;
		VideoFormat = DISPHSTX_FORMAT_2_PAL;
		VideoPal = DispHstxPal2b;
		break;

	// 640x300, 2 bits per pixel, colors black/blue/red/white
	case VMODE_640x300x2col1:
		Width = 640;
		WidthByte = 640/4;
		Height = 300;
		Hdbl = 1;
		Vdbl = 2;
		VideoFormat = DISPHSTX_FORMAT_2_PAL;
		VideoPal = DispHstxPal2bcol1;
		break;

	// 640x300, 2 bits per pixel, colors black/green/red/yellow
	case VMODE_640x300x2col2:
		Width = 640;
		WidthByte = 640/4;
		Height = 300;
		Hdbl = 1;
		Vdbl = 2;
		VideoFormat = DISPHSTX_FORMAT_2_PAL;
		VideoPal = DispHstxPal2bcol2;
		break;

	// 320x300, 4 bits per pixel, 16 gray levels
	case VMODE_320x300x4bw:
		Width = 320;
		WidthByte = 320/2;
		Height = 300;
		Hdbl = 2;
		Vdbl = 2;
		VideoFormat = DISPHSTX_FORMAT_4_PAL;
		VideoPal = DispHstxPal4b;
		break;

	// 320x300, 4 bits per pixel, 16 colors in format YRGB1111
	case VMODE_320x300x4col:
		Width = 320;
		WidthByte = 320/2;
		Height = 300;
		Hdbl = 2;
		Vdbl = 2;
		VideoFormat = DISPHSTX_FORMAT_4_PAL;
		VideoPal = DispHstxPal4bcol;
		break;

	// 320x150, 8 bits per pixel, 256 colors in format RGB332
	case VMODE_320x150x8:
		Width = 320;
		WidthByte = 320;
		Height = 150;
		Hdbl = 2;
		Vdbl = 4;
		VideoFormat = DISPHSTX_FORMAT_8_PAL;
		VideoPal = DispHstxPal8b;
		break;

	// 160x300, 8 bits per pixel, 256 colors in format RGB332
	case VMODE_160x300x8:
		Width = 160;
		WidthByte = 160;
		Height = 300;
		Hdbl = 4;
		Vdbl = 2;
		VideoFormat = DISPHSTX_FORMAT_8_PAL;
		VideoPal = DispHstxPal8b;
		break;

	// 160x150, 16 bits per pixel, format RGB565
	case VMODE_160x150x16:
		Width = 160;
		WidthByte = 160;
		Height = 150;
		Hdbl = 4;
		Vdbl = 4;
		VideoFormat = DISPHSTX_FORMAT_16;
		VideoPal = NULL;
		break;

	default:
		Vmode = VMODE_OFF;
		break;
	}
}

// Display modes
//#define DISPHSTX_DISPMODE_NONE	0	// display mode - none (use auto-detection)
//#define DISPHSTX_DISPMODE_DVI		1	// display mode - DVI (HDMI)
//#define DISPHSTX_DISPMODE_VGA		2	// display mode - VGA

// Videomodes
//#define VMODE_OFF		0	// disable output
//#define VMODE_640x600x1	1	// 640x600, 1 bit per pixel, B&W levels
//#define VMODE_640x300x2bw	2	// 640x300, 2 bits per pixel, 4 gray levels
//#define VMODE_640x300x2col1	3	// 640x300, 2 bits per pixel, colors black/blue/red/white
//#define VMODE_640x300x2col2	4	// 640x300, 2 bits per pixel, colors black/green/red/yellow
//#define VMODE_320x300x4bw 	5	// 320x300, 4 bits per pixel, 16 gray levels
//#define VMODE_320x300x4col 	6	// 320x300, 4 bits per pixel, 16 colors in format YRGB1111
//#define VMODE_320x150x8 	7	// 320x150, 8 bits per pixel, 256 colors in format RGB332
//#define VMODE_160x300x8 	8	// 160x300, 8 bits per pixel, 256 colors in format RGB332
//#define VMODE_160x150x16	9	// 160x150, 16 bits per pixel, format RGB565

// initialize videomode
//  dispmode ... display mode DISPHSTX_DISPMODE_*
//  videomode ... video mode VMODE_*
void InitVideomode(int dispmode, int videomode)
{
	// check current mode
	if ((dispmode == DispHstxDispMode) && (videomode == Vmode)) return;

	// stop current videomode
	DispHstxAllTerm();

	// prepare videomode parameters
	PrepVideomode(videomode);
	if (Vmode == VMODE_OFF) return;

	// auto-detect display mode
	if (dispmode == DISPHSTX_DISPMODE_NONE)
	{
		dispmode = DispHstxAutoDispSel();
	}

	// initialize videomode descriptor
	VModeStateInx = 0;
	int res = DispHstxVModeInitSimple(
		&VModeState[0],		// videomode state descriptor
		&VModeTime,		// videomode timings descriptor
		Hdbl,			// horizontal divide
		Vdbl,			// vertical divide
		VideoFormat,		// pixel format
		FrameBuf,		// frame buffer (NULL=create new one)
		VideoPal,		// pointer to palettes in RGB565 format (should be in RAM)
		NULL,			// pointer to font (if text format)
		0);			// height of font

	// emergency error
	while (res != DISPHSTX_ERR_OK) { GPIO_Flip(LED_PIN); WaitMs(200); }

	// start videomode
	DispHstxSelDispMode(dispmode, &VModeState[0]);
}

// Selecting new video mode without losing synchronization
void SetVideomode(int videomode)
{
	// check current mode
	if (videomode == Vmode) return;

	// prepare videomode parameters
	PrepVideomode(videomode);
	if (Vmode == VMODE_OFF) return;

	// new descriptor
	int inx = (VModeStateInx + 1) & 1;
	VModeStateInx = inx;
	sDispHstxVModeState* vs = &VModeState[inx];

	// initialize videomode descriptor
	int res = DispHstxVModeInitSimple(
		vs,			// videomode state descriptor
		&VModeTime,		// videomode timings descriptor
		Hdbl,			// horizontal divide
		Vdbl,			// vertical divide
		VideoFormat,		// pixel format
		FrameBuf,		// frame buffer (NULL=create new one)
		VideoPal,		// pointer to palettes in RGB565 format (should be in RAM)
		NULL,			// pointer to font (if text format)
		0);			// height of font

	// emergency error
	while (res != DISPHSTX_ERR_OK) { GPIO_Flip(LED_PIN); WaitMs(200); }

	// exchange videomode descriptor
	DispHstxExchange(vs);
}
