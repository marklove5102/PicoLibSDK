
// ****************************************************************************
//
//                   HDMI and VGA display driver for Vzlet Z256
//                               Draw test pattern
//
// ****************************************************************************

#include "../include.h"

// draw test pattern
void TestPattern()
{
	const u8* s;

	switch (Vmode)
	{
	// 640x600, 1 bit per pixel, B&W levels
	case VMODE_640x600x1:
		s = Img640x600x1;
		break;

	// 640x300, 2 bits per pixel, 4 gray levels
	case VMODE_640x300x2bw:
		s = Img640x300x2bw;
		break;

	// 640x300, 2 bits per pixel, colors black/blue/red/white
	case VMODE_640x300x2col1:
		s = Img640x300x2col1;
		break;

	// 640x300, 2 bits per pixel, colors black/green/red/yellow
	case VMODE_640x300x2col2:
		s = Img640x300x2col2;
		break;

	// 320x300, 4 bits per pixel, 16 gray levels
	case VMODE_320x300x4bw:
		s = Img320x300x4bw;
		break;

	// 320x300, 4 bits per pixel, 16 colors in format YRGB1111
	case VMODE_320x300x4col:
		s = Img320x300x4col;
		break;

	// 320x150, 8 bits per pixel, 256 colors in format RGB332
	case VMODE_320x150x8:
		s = Img320x150x8;
		break;

	// 160x300, 8 bits per pixel, 256 colors in format RGB332
	case VMODE_160x300x8:
		s = Img160x300x8;
		break;

	// 160x150, 16 bits per pixel, format RGB565
	case VMODE_160x150x16:
		s = (const u8*)Img160x150x16;
		break;

	default:
		return;
	}

	memcpy(FrameBuf, s, 48000);

	// draw circle
	int r = Width;
	if (r > Height) r = Height;
	r = r * 3/8;
	DrawCircleInv(Width/2, Height/2, r);
}
