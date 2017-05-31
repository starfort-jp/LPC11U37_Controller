/**
 ===============================================================================
 Project Name	: LPC11U37_Controller
 Name			: main.c
 @author      	: H.Gotou
 @version     	: 0.00.01
 Copyright   	: (c)H.Gotou, 2016
// Description 	: main routine
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC11Uxx.h"
#endif

#include <cr_section_macros.h>

// User Definition Include Files
#include "gpio.h"
#include "systick.h"
#include "i2c.h"
#include "cappuccino.h"
#include "i2clcd.h"
#include "xprintf.h"

// ROM function
#include "mw_usbd_rom_api.h"
#include "usb_rom_func.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>  // for C99
#include <stdbool.h> // for boolean
//---

// TODO: insert other definitions and declarations here
void init(void) {
// Initialization
	SystemInit();
	GPIOInit();
	// Initialize SysTick
	SysTick_Config(SystemCoreClock / 1000);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6);
	// initialize I2C
	if (I2CInit((uint32_t) I2CMASTER) == false) {
		while (true)
			;							// Fatal Error -> Infinity Loop
	}
	// initialize I2C_LCD
	Systick_Start();
	while (true) {
		if (i2c_lcd_init(0x27) == 0) {
			break;	// Abort - Success
		}
		if (Systick_Gettime() > 3000) {
			break;	// Abort - Timeout
		}
	}
	Systick_Stop();

	// initialize USB CDC
	USB_CDC_Init();

	// initialize xprintf
	// xprintf output to I2C_LCD/UART
	xfunc_out = (void (*)(unsigned char)) i2c_lcd_set_data;  //I2C_LCD
}

long loop(long xTriggered) {
	unsigned long xUserSwitch_Status;
	int xLp;
	char xText[17];
	unsigned char yText[] = "   ";
	unsigned char *pText;
//Test I/O port
	xUserSwitch_Status = GPIOGetPinValue(0x00, 0x01);
	if ((xUserSwitch_Status & 0x01) == 0x01) {
		HIGH(1, 19);
		i2c_lcd_backlight(false);
		if (xTriggered == 1) {
			i2c_lcd_clear();
			i2c_lcd_put_icon(0x0d, 0x00);
			i2c_lcd_put_icon(0x00, 0x10);
			i2c_lcd_set_cmd(0x80);	    // set address to 1st line
					/*
					 strcpy(xText, "UserSW > OFF");
					 for (xLp = 0; xLp <= 16; xLp++)
					 {
					 yText[xLp] = (unsigned char)xText[xLp];
					 }
					 pText = yText;
					 i2c_lcd_put_string(pText);
					 */
			xprintf("%s", "UserSW > OFF ");
			xTriggered = 0;
		}
	} else {
		LOW(1, 19);
		i2c_lcd_backlight(true);
		if (xTriggered == 0) {
			i2c_lcd_clear();
			i2c_lcd_put_icon(0x00, 0x00);
			i2c_lcd_put_icon(0x0d, 0x1e);
			i2c_lcd_set_cmd(0x80);	    // set address 1st line
					/*
					 strcpy(xText, "UserSW > ON");
					 for (xLp = 0; xLp <= 16; xLp++)
					 {
					 yText[xLp] = (unsigned char)xText[xLp];
					 }
					 pText = yText;
					 i2c_lcd_put_string(pText);
					 */
			xprintf("%s", "UserSW > ON");
			xTriggered = 1;
		}
	};
	yText[0] = xData;
	pText = yText;
	i2c_lcd_set_cmd(0xc0);	    // set address to 2nd line
	i2c_lcd_put_string(pText);
	return xTriggered;
}

int main(void) {
	int xLp;
	char xText[17];
	unsigned char yText[17];
	unsigned char *pText;
	long xTriggered;
	long xReturn;
	// TODO: insert code here
	init();
// Port Test
	LPC_GPIO->DIR[1] |= (1 << 19);
	LPC_GPIO->SET[1] = (1 << 19);
//	LPC_GPIO->CLR[1] = (1<<19);
//---

// Start-Up Display
	i2c_lcd_backlight(true);  // Back Light ON
//	  i2c_lcd_put_hex8(255);	//Display Hex 00h..FFh
//	  i2c_lcd_put_decimal(-2147483647);	//Display Decimal -2147483647..2147483647
	strcpy(xText, "Welcome LPC11U37");
	for (xLp = 0; xLp <= 16; xLp++) {
		yText[xLp] = (unsigned char) xText[xLp];
	}
	pText = yText;
	i2c_lcd_put_string(pText);
	i2c_lcd_set_cmd(0xc0);	    // set address 2nd line
	strcpy(xText, "OK >");
	for (xLp = 0; xLp <= 16; xLp++) {
		yText[xLp] = (unsigned char) xText[xLp];
	}
	pText = yText;
	i2c_lcd_put_string(pText);
	wait_ms(1000);
	i2c_lcd_backlight(false);  // Back Light OFF
//---

	// Force the counter to be placed into memory
//    volatile static int i = 0 ;
	// Enter an infinite loop, just incrementing a counter
	xTriggered = 1;
	while (true) {
		xReturn = loop(xTriggered);
		xTriggered = xReturn;
	}
	return 0;
}
