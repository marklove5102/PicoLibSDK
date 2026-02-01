
// ****************************************************************************
//
//                   HDMI and VGA display driver for Vzlet Z256
//                                Keyboard
//
// ****************************************************************************

#include "../include.h"

// pins of buttons
const int BtnPinList[BTN_NUM] = {
	BTN_2_PIN,	// X (menu up/left)
	BTN_3_PIN,	// Y (menu down/right)
	BTN_4_PIN,	// A (Enter)
	BTN_5_PIN,	// B (Esc)
};

// button last press time
u32 BtnLastPress[BTN_NUM];

// key code (BTN_NO = no key)
u8 BtnKey = BTN_NO;

// initialize buttons
void InitBtn()
{
	int i;
	for (i = 0; i < BTN_NUM; i++)
	{
		// get pin
		int pin = BtnPinList[i];

		// initialize GPIO pin to input
		GPIO_Init(pin);

		// setup pull-up
		GPIO_PullUp(pin);
	}
}

// scan buttons
void ScanBtn()
{
	int i;
	u32 t = Time();
	for (i = 0; i < BTN_NUM; i++)
	{
		// get pin
		int pin = BtnPinList[i];

		// check if button is pressed
		if (GPIO_In(pin) == 0)
		{
			// button is pressed for first time - save key code into key buffer
			if ((u32)(t - BtnLastPress[i]) > 100000) BtnKey = (u8)(i + 1);

			// save new last press time
			BtnLastPress[i] = t;
		}
	}
}

// get button (BTN_NO = no key)
u8 GetBtn()
{
	// scan buttons
	ScanBtn();

	// get button
	u8 btn = BtnKey;
	BtnKey = BTN_NO;
	return btn;
}
