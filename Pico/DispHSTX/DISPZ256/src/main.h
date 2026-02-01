
// ****************************************************************************
//
//                   HDMI and VGA display driver for Vzlet Z256
//                                 Main code
//
// ****************************************************************************

#ifndef _MAIN_H
#define _MAIN_H

// COM port (9600 Baud, 8 bits, no parity, 1 stop bit)
#define UART_INX	0	// UART index 0=UART0 or 1=UART1
#define UART_PIN_TX	0	// UART TX pin
#define UART_PIN_RX	1	// UART RX pin
#define UART_BAUD	9600	// UART baudrate (1 ms per sample)

// input pins
#define PIN_V1		2	// V1 (normal state: 0 black, 1 white)
#define PIN_V2		3	// V2 (normal state: 0 black, 1 white)
#define PIN_CLK		4	// CLK 12.5MHz (normal state: pixel = HIGH + LOW)
#define PIN_HSYNC	5	// HSYNC (normal state: negative polarity sync=LOW, blank=HIGH)
#define PIN_VSYNC	6	// VSYNC (normal state: negative polarity sync=LOW, blank=HIGH)

// input polarity
#define PIN_V1_INV	0	// 0=V1 normal (0 black, 1 white), 1=V1 inverted (1 black, 0 white)
#define PIN_V2_INV	0	// 0=V2 normal (0 black, 1 white), 1=V2 inverted (1 black, 0 white)
#define PIN_CLK_INV	0	// 0=CLK normal (pixel = HIGH + LOW), 1=CLK inverted (pixel = LOW + HIGH)
#define PIN_HSYNC_INV	0	// 0=HSYNC negative (sync=LOW, blank=HIGH), 1=HSYNC positive (sync=HIGH, blank=LOW)
#define PIN_VSYNC_INV	0	// 0=VSYNC negative (sync=LOW, blank=HIGH), 1=VSYNC positive (sync=HIGH, blank=LOW)

// input signal timings
#define VBACK		2	// vertical back porch in number of lines (start of image after end of VSYNC pulse
#define HBACK		70	// horizontal back porch in number of pixels (start of image after end of HSYNC pulse)

// pins of buttons
#define BTN_2_PIN	8	// X (menu up/left)
#define BTN_3_PIN	9	// Y (menu down/right)
#define BTN_4_PIN	10	// A (Enter)
#define BTN_5_PIN	11	// B (Esc)
#define BTN_NUM		4	// number of buttons

// indices of buttons
#define BTN_X		1	// X (menu up/left)
#define BTN_Y		2	// Y (menu down/right)
#define BTN_A		3	// A (Enter)
#define BTN_B		4	// B (Esc)
#define BTN_NO		0	// no key

#define FRAMESIZE	48000	// size of frame buffer (= 640x300/4)

extern ALIGNED u8 FrameBuf[FRAMESIZE]; // pointer to display frame buffer
extern int Vmode;	// current videomode VMODE_*
extern int Width;	// current videomode width in pixels
extern int WidthByte;	// current videomode length of line in bytes
extern int Height;	// current videomode height
extern int Hdbl;	// horizontal divide
extern int Vdbl;	// vertical divide
extern int VideoFormat; // video format
extern u16* VideoPal;	// video palettes
extern sDispHstxVModeState VModeState[2]; // videomode descriptors
extern int VModeStateInx; // index of current videomode descriptor

// draw test pattern
void TestPattern();

// test patterns

// format: 1-bit pixel graphics
// image width: 640 pixels
// image height: 600 lines
// image pitch: 80 bytes
extern const u8 Img640x600x1[FRAMESIZE] __attribute__ ((aligned(4)));

// format: 2-bit paletted pixel graphics
// image width: 640 pixels
// image height: 300 lines
// image pitch: 160 bytes
extern const u16 Img640x300x2col1_Pal[4] __attribute__ ((aligned(4)));
extern const u8 Img640x300x2col1[FRAMESIZE] __attribute__ ((aligned(4)));
extern const u16 Img640x300x2col2_Pal[4] __attribute__ ((aligned(4)));
extern const u8 Img640x300x2col2[FRAMESIZE] __attribute__ ((aligned(4)));
extern const u16 Img640x300x2bw_Pal[4] __attribute__ ((aligned(4)));
extern const u8 Img640x300x2bw[FRAMESIZE] __attribute__ ((aligned(4)));

// format: 4-bit paletted pixel graphics
// image width: 320 pixels
// image height: 300 lines
// image pitch: 160 bytes
extern const u16 Img320x300x4col_Pal[16] __attribute__ ((aligned(4)));
extern const u8 Img320x300x4col[FRAMESIZE] __attribute__ ((aligned(4)));
extern const u16 Img320x300x4bw_Pal[16] __attribute__ ((aligned(4)));
extern const u8 Img320x300x4bw[FRAMESIZE] __attribute__ ((aligned(4)));

// format: 8-bit paletted pixel graphics
// image width: 320 pixels
// image height: 150 lines
// image pitch: 320 bytes
extern const u16 Img320x150x8_Pal[256] __attribute__ ((aligned(4)));
extern const u8 Img320x150x8[FRAMESIZE] __attribute__ ((aligned(4)));

// format: 8-bit paletted pixel graphics
// image width: 160 pixels
// image height: 300 lines
// image pitch: 160 bytes
extern const u16 Img160x300x8_Pal[256] __attribute__ ((aligned(4)));
extern const u8 Img160x300x8[FRAMESIZE] __attribute__ ((aligned(4)));

// format: 16-bit pixel graphics
// image width: 160 pixels
// image height: 150 lines
// image pitch: 320 bytes
extern const u16 Img160x150x16[FRAMESIZE/2] __attribute__ ((aligned(4)));

// Display modes
//#define DISPHSTX_DISPMODE_NONE	0	// display mode - none (use auto-detection)
//#define DISPHSTX_DISPMODE_DVI		1	// display mode - DVI (HDMI)
//#define DISPHSTX_DISPMODE_VGA		2	// display mode - VGA
#define DISPMODE	DISPHSTX_DISPMODE_VGA	// default display mode

// Videomodes
#define VMODE_OFF		0	// disable output
#define VMODE_640x600x1		1	// 640x600, 1 bit per pixel, B&W levels
#define VMODE_640x300x2bw	2	// 640x300, 2 bits per pixel, 4 gray levels
#define VMODE_640x300x2col1	3	// 640x300, 2 bits per pixel, colors black/blue/red/white
#define VMODE_640x300x2col2	4	// 640x300, 2 bits per pixel, colors black/green/red/yellow
#define VMODE_320x300x4bw 	5	// 320x300, 4 bits per pixel, 16 gray levels
#define VMODE_320x300x4col 	6	// 320x300, 4 bits per pixel, 16 colors in format YRGB1111
#define VMODE_320x150x8 	7	// 320x150, 8 bits per pixel, 256 colors in format RGB332
#define VMODE_160x300x8 	8	// 160x300, 8 bits per pixel, 256 colors in format RGB332
#define VMODE_160x150x16	9	// 160x150, 16 bits per pixel, format RGB565
#define VMODE_MAX		9	// max. videomode
#define VMODE		VMODE_640x300x2bw	// default videomode

// initialize videomode
//  dispmode ... display mode DISPHSTX_DISPMODE_*
//  videomode ... video mode VMODE_*
void InitVideomode(int dispmode, int videomode);

// Selecting new video mode without losing synchronization
void SetVideomode(int videomode);

// initialize buttons
void InitBtn();

// get button (BTN_NO = no key)
u8 GetBtn();

#endif // _MAIN_H
