/********************************************************************************
 * i2clcd.c:  I2C_LCD C file for Strawberry Linux SB1602BW (Extension Mode)
 * Copyright(C) 2016, Starfort, All rights reserved.
 *  <History>
 *  date      : rev. : comment
 *  2016/10/20: 1.00 : Release
 * ******************************************************************************/
#include "stdbool.h"	// for bool type
#include "LPC11uxx.h"	// LPC11uxx Peripheral Registers
#include "gpio.h"
#include "i2c.h"
#include "type.h"
#include "cappuccino.h"
#include "i2clcd.h"
#include "systick.h"

#define I2C_LCD_RST      25   // PIO1_25
#define I2C_LCD_BK_LIGHT  3   // PIO0_3
#define I2C_LCD_ADDR   0x7c   // LCD I2C Address 0x7c
#define I2C_LCD_CMD    0x00   // LCD Set Command
#define I2C_LCD_WD     0x40   // LCD Set Data

extern volatile uint32_t I2CCount;
extern volatile uint8_t I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CMasterState;
extern volatile uint32_t I2CReadLength, I2CWriteLength;

void i2c_lcd_backlight(bool sw) 	// I2C_LCD Back Light Control -> PIO0_3
{
	OUTPUT(0, I2C_LCD_BK_LIGHT);
//	IOSET(0, I2C_LCD_BK_LIGHT, !sw);	//active low
	if (sw)
	{
		LOW(0, I2C_LCD_BK_LIGHT)
	}
	else
	{
		HIGH(0, I2C_LCD_BK_LIGHT)
	}
}

uint8_t i2c_lcd_set_cmd(unsigned char db)   // Write LCD Instruction (same as HD44780 RS=0)
{
	return i2c_write(I2C_LCD_ADDR, I2C_LCD_CMD, db);
}

uint8_t i2c_lcd_set_data(unsigned char db)  // Write LCD Data (same as HD44780 RS=1)
{
	return i2c_write(I2C_LCD_ADDR, I2C_LCD_WD, db);
}

uint32_t i2c_lcd_init(unsigned char contrast)  //LCD Initialization
{
	OUTPUT(1, I2C_LCD_RST);	// set I2C_LCD Reset pin output
	LOW(1, I2C_LCD_RST);	// activate Reset to Low
	wait_ms(10);			// wait 10ms
	HIGH(1, I2C_LCD_RST);	// inactive Reset to High
	wait_ms(40);			// wait 40ms

	i2c_lcd_set_cmd(0x39);	// Function Set -> enable instruction table select
	i2c_lcd_set_cmd(0x14);	// Internal OSC Frequency -> 1/4 bias
	i2c_lcd_set_cmd(0x6C);	// Follower Control -> circuit on, amplified ratio '100'
	i2c_lcd_set_cmd(0x5C | ((contrast >> 4) & 0x3));	// Power/ICONcontrol/ContrastSet -> Icon ON, C4-C5
	i2c_lcd_set_cmd(0x70 | (contrast & 0xF));	// ContrastSet C0-C3
	i2c_lcd_set_cmd(0x0c);	// Display ON/OFF -> entire ON
	i2c_lcd_set_cmd(0x01);	// Clear Display -> Clear
	wait_ms(2);		// Wait 2msec

	i2c_lcd_set_cmd(0x38);	// Function Set -> disable instruction table select
	wait_ms(300);	// wait 300ms

  	return 0;
}

uint8_t i2c_lcd_clear()  // Clear Display
{
	uint8_t res;
    res = i2c_lcd_set_cmd(0x01);	// Clear Display -> Clear
    if (res == 1)
    {
    	return res;
    }
    wait_ms(2);		// Wait 2msec
    return 0;
}

void i2c_lcd_put_icon(unsigned char Xaddr, unsigned char Xdata)
{
	i2c_lcd_set_cmd(0x39);	//  Function Set -> enable instruction table select
	i2c_lcd_set_cmd(0x40 | Xaddr);	// Set Icon Address
    i2c_lcd_set_data(Xdata);		// Set Icon Data
	wait_ms(2);				// Wait 2msec
	i2c_lcd_set_cmd(0x38);	// Function Set -> disable instruction table select
}

uint8_t i2c_lcd_put_string(unsigned char *s)  // Put string
{
    uint8_t res;
	while (*s)
	{
		res = i2c_lcd_set_data(*s++);
        if (res < 0)
        {
        	return res;
        }
	}
	return 0;
}

static const unsigned char hex[16]="0123456789ABCDEF";

void i2c_lcd_put_hex8(unsigned char c)	//caution! limit 00h..FFh
{
	i2c_lcd_set_data(hex[c>>4]);
	i2c_lcd_set_data(hex[c&15]);
}

static unsigned char _hex10[10+1+2];

unsigned int i2c_lcd_put_decimal(long d)	//caution! limit -2147483647..2147483647
{
	unsigned char *p;
	unsigned char n;
	if (d < 0)
	{
		i2c_lcd_set_data('-');
		return i2c_lcd_put_decimal(-d);
	}
	if ( !d )
	{
		i2c_lcd_set_data('0');
		return 1;
	}
	n = 0;
	p = _hex10 + sizeof(_hex10);
	*--p = '\0';
	while (d)
	{
		*--p = (d % 10)+'0';
		d /= 10;
	}
	while (*p)
	{
		i2c_lcd_set_data(*p++);
		n++;
	}
	return n;
}
