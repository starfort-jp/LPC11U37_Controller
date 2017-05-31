/********************************************************************************
 * i2clcd.h:  I2C_LCD C-header file for Strawberry Linux SB1602BW (Extension Mode)
 * Copyright(C) 2016, Starfort, All rights reserved.
 *  <History>
 *  date      : rev. : comment
 *  2016/10/20: 1.00 : Release
 * ******************************************************************************/
#include "stdbool.h"
extern void i2c_lcd_backlight(bool sw);
extern uint8_t i2c_lcd_set_cmd(unsigned char db);
extern uint8_t i2c_lcd_set_data(unsigned char db);
extern uint32_t i2c_lcd_init( unsigned char contrast );
extern uint8_t i2c_lcd_clear();
extern void i2c_lcd_put_icon(unsigned char Xaddr, unsigned char Xdata);
extern uint8_t i2c_lcd_put_string(unsigned char *s);
extern void i2c_lcd_put_hex8(unsigned char c);
extern unsigned int i2c_lcd_put_decimal(long d);
