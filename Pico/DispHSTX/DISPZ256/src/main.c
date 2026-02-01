
// ****************************************************************************
//
//                   HDMI and VGA display driver for Vzlet Z256
//                                 Main code
//
// ****************************************************************************

#include "../include.h"

// input from pins
#if USE_GPIOCOPROC	// use GPIO coprocessor (only RP2350 ARM; sdk_gpio_coproc.h)
#define IN_V() ((GPIOC_InAllL() >> PIN_V1) & 3)
#else
#define IN_V() ((GPIOC_InAll() >> PIN_V1) & 3)
#endif

#define IN_CLK() GPIO_In(PIN_CLK)
#define IN_HSYNC() GPIO_In(PIN_HSYNC)
#define IN_VSYNC() GPIO_In(PIN_VSYNC)

// wait for CLK falling edge (middle of pixel)
#define CLKWAIT()	{ cb(); while (IN_CLK() == 0) {}; cb(); while (IN_CLK() != 0) {}; cb(); }

// wait for HSYNC falling edge (end of HSYNC pulse)
#define HWAIT()		{ cb(); while (IN_HSYNC() != 0) {}; cb(); while (IN_HSYNC() == 0) {}; cb(); }

// initialize input signal pins
void InitIn()
{
	GPIO_Init(PIN_V1);
	GPIO_NoPull(PIN_V1);
#if PIN_V1_INV
	GPIO_InOverInvert(PIN_V1);
#endif

	GPIO_Init(PIN_V2);
	GPIO_NoPull(PIN_V2);
#if PIN_V2_INV
	GPIO_InOverInvert(PIN_V2);
#endif

	GPIO_Init(PIN_CLK);
	GPIO_NoPull(PIN_CLK);
#if PIN_CLK_INV
	GPIO_InOverInvert(PIN_CLK);
#endif

	GPIO_Init(PIN_HSYNC);
	GPIO_PullUp(PIN_HSYNC);
#if PIN_HSYNC_INV
	GPIO_InOverInvert(PIN_HSYNC);
#endif

	GPIO_Init(PIN_VSYNC);
	GPIO_PullUp(PIN_VSYNC);
#if PIN_VSYNC_INV
	GPIO_InOverInvert(PIN_VSYNC);
#endif
}

// initialize UART port
void ComInit()
{
	// UART init - 9600 Baud, 8 bits, no parity, 1 stop bit
	UART_Init(UART_INX, UART_BAUD, 8, UART_PARITY_NONE, 1, False);

	// setup GPIO pins
	GPIO_Fnc(UART_PIN_TX, GPIO_FNC_UART);
	GPIO_Fnc(UART_PIN_RX, GPIO_FNC_UART);

	// flush false receiving
	WaitUs(100);
	UART_RecvFlush(UART_INX);
}

int main()
{
	u8* d;
	int i, j, b, b1, b2, w;

	// disable interrupts (grabbing must not be interrupted)
	di();

	// initialize buttons
	InitBtn();

	// initialize UART port
	ComInit();

	// initialize input signal pins
	InitIn();

	// initialize videomode
	InitVideomode(DISPMODE, VMODE);

	// draw test pattern
	TestPattern();

	// main loop
	while (True)
	{
		// wait for start of VSYNC signal
		cb();
		while (IN_VSYNC() != 0)
		{
			// get button (BTN_NO = no key)
			switch (GetBtn())
			{
			case BTN_X:	// previous videomode
				i = Vmode - 1;
				if (i <= 0) i = VMODE_MAX;
				SetVideomode(i);
				TestPattern();
				break;

			case BTN_Y:	// next videomode
				i = Vmode + 1;
				if (i > VMODE_MAX) i = 1;
				SetVideomode(i);
				TestPattern();
				break;
			}

			// receiving COM
			if (UART_RecvReady(UART_INX))
			{
				b = UART_RecvChar(UART_INX) - '0';
				if (b == -1) // '/' ... get videomode
				{
					UART_SendChar(UART_INX, Vmode + '0');
				}
				else if (b == 0)
				{
					SetVideomode(VMODE_160x150x16); // 160x150, 16 bits per pixel, format RGB565
					TestPattern();
					DrawText2("OFFLINE", (160-7*16)/2, (150-16)/2, COL_WHITE);
				}
				else if ((u32)b <= VMODE_MAX)
				{
					SetVideomode(b);
					TestPattern();
				}
			}
		}
		cb();

		// wait for end of VSYNC signal
		while (IN_VSYNC() == 0) {};
		cb();

		// skip vertical back porch
		for (i = VBACK; i > 0; i--) HWAIT();

		// image scanlines
		d = FrameBuf;	// destination frame buffer
		switch (Vmode)
		{
		// 640x600, 1 bit per pixel, B&W levels
		case VMODE_640x600x1:

			// image loop
			w = 640/8;
			for (i = 300; i > 0; i--)
			{
				// wait for HSYNC falling edge (end of HSYNC pulse)
				HWAIT();

				// skip horizontal back porch
				for (j = HBACK; j > 0; j--) CLKWAIT();

				// load pixels
				for (j = 640/8; j > 0; j--)
				{
					// load bit 7
					CLKWAIT();
					b = IN_V();
					b1 = (IN_V() & 1) << 7;
					b2 = (IN_V() & 2) << 6;

					// load bit 6
					CLKWAIT();
					b = IN_V();
					b1 |= (IN_V() & 1) << 6;
					b2 |= (IN_V() & 2) << 5;

					// load bit 5
					CLKWAIT();
					b = IN_V();
					b1 |= (IN_V() & 1) << 5;
					b2 |= (IN_V() & 2) << 4;

					// load bit 4
					CLKWAIT();
					b = IN_V();
					b1 |= (IN_V() & 1) << 4;
					b2 |= (IN_V() & 2) << 3;

					// load bit 3
					CLKWAIT();
					b = IN_V();
					b1 |= (IN_V() & 1) << 3;
					b2 |= (IN_V() & 2) << 2;

					// load bit 2
					CLKWAIT();
					b = IN_V();
					b1 |= (IN_V() & 1) << 2;
					b2 |= (IN_V() & 2) << 1;

					// load bit 1
					CLKWAIT();
					b = IN_V();
					b1 |= (IN_V() & 1) << 1;
					b2 |= (IN_V() & 2);

					// load bit 0
					CLKWAIT();
					b = IN_V();
					b1 |= (IN_V() & 1);
					b2 |= (IN_V() & 2) >> 1;

					// store byte
					*d = (u8)b1;
					d[w] = (u8)b2;
					d++;
				}
				d += w;
			}
			break;

		// 640x300, 2 bits per pixel, 4 gray levels
		case VMODE_640x300x2bw:
		// 640x300, 2 bits per pixel, colors black/blue/red/white
		case VMODE_640x300x2col1:
		// 640x300, 2 bits per pixel, colors black/green/red/yellow
		case VMODE_640x300x2col2:
		// 320x300, 4 bits per pixel, 16 gray levels
		case VMODE_320x300x4bw:
		// 320x300, 4 bits per pixel, 16 colors in format YRGB1111
		case VMODE_320x300x4col:
		// 160x300, 8 bits per pixel, 256 colors in format RGB332
		case VMODE_160x300x8:

			// image loop
			for (i = 300; i > 0; i--)
			{
				// wait for HSYNC falling edge (end of HSYNC pulse)
				HWAIT();

				// skip horizontal back porch
				for (j = HBACK; j > 0; j--) CLKWAIT();

				// load pixels
				for (j = 640/4; j > 0; j--)
				{
					// load bits 7,6
					CLKWAIT();
					b = IN_V() << 6;

					// load bits 5,4
					CLKWAIT();
					b |= IN_V() << 4;
					
					// load bits 3,2
					CLKWAIT();
					b |= IN_V() << 2;

					// load bits 1,0
					CLKWAIT();
					b |= IN_V();

					// store byte
					*d++ = (u8)b;
				}
			}
			break;

		// 320x150, 8 bits per pixel, 256 colors in format RGB332
		case VMODE_320x150x8:

			// image loop
			for (i = 150; i > 0; i--)
			{
				// wait for HSYNC falling edge (end of HSYNC pulse)
				HWAIT();

				// skip horizontal back porch
				for (j = HBACK; j > 0; j--) CLKWAIT();

				// load pixels - odd scanline (higher 4 bits)
				for (j = 640/2; j > 0; j--)
				{
					// load bits 7,6
					CLKWAIT();
					b = IN_V() << 6;

					// load bits 5,4
					CLKWAIT();
					b |= IN_V() << 4;

					// store byte
					*d = (*d & 0x0f) | b;
					d++;
				}
				d -= 320;

				// wait for HSYNC falling edge (end of HSYNC pulse)
				HWAIT();

				// skip horizontal back porch
				for (j = HBACK; j > 0; j--) CLKWAIT();

				// load pixels - even scanline (lower 4 bits)
				for (j = 640/2; j > 0; j--)
				{
					// load bits 3,2
					CLKWAIT();
					b = IN_V() << 2;

					// load bits 0,1
					CLKWAIT();
					b |= IN_V();

					// store byte
					*d = (*d & 0xf0) | b;
					d++;
				}
			}
			break;

		// 160x150, 16 bits per pixel, format RGB565
		case VMODE_160x150x16:

			// image loop
			for (i = 150; i > 0; i--)
			{
				// wait for HSYNC falling edge (end of HSYNC pulse)
				HWAIT();

				// skip horizontal back porch
				for (j = HBACK; j > 0; j--) CLKWAIT();

				// load pixels - even scanline (lower 8 bits)
				for (j = 640/4; j > 0; j--)
				{
					// load bits 7,6
					CLKWAIT();
					b = IN_V() << 6;

					// load bits 5,4
					CLKWAIT();
					b |= IN_V() << 4;

					// load bits 3,2
					CLKWAIT();
					b |= IN_V() << 2;

					// load bits 0,1
					CLKWAIT();
					b |= IN_V();

					// store byte
					*d = b;
					d += 2;
				}
				d -= 320;

				// wait for HSYNC falling edge (end of HSYNC pulse)
				HWAIT();

				// skip horizontal back porch
				for (j = HBACK; j > 0; j--) CLKWAIT();

				// load pixels - odd scanline (higher 8 bits)
				for (j = 640/4; j > 0; j--)
				{
					// load bits 7,6
					CLKWAIT();
					b = IN_V() << 6;

					// load bits 5,4
					CLKWAIT();
					b |= IN_V() << 4;

					// load bits 3,2
					CLKWAIT();
					b |= IN_V() << 2;

					// load bits 0,1
					CLKWAIT();
					b |= IN_V();

					// store byte
					d[1] = b;
					d += 2;
				}
			}
			break;
		}
	}
}
