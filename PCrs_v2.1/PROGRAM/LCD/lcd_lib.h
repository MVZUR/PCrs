/*
 * lcd_lib.h
 *
 *  Created on: 30 sie 2019
 *      Author: Maksymilian
 */

#ifndef LCD_LCD_LIB_H_
#define LCD_LCD_LIB_H_

#include <avr/eeprom.h>


//***************************************
//										*
//			S E T T I N G S				*
// 										*
// 		1 - use, 0 - do not use			*
//								  		*
//*****************************************************************************************

#define USE_LCD_LOCATE			1		// <--- set cursor in position Y, X

#define USE_LCD_CHAR 			1		// <--- send single char

#define USE_LCD_STR_P 			1		// <--- send string from FLASH
#define USE_LCD_STR_E 			1		// <--- send string from EEPROM

#define USE_LCD_INT 			1		// <--- send int
#define USE_LCD_HEX 			1		// <--- send hex

#define USE_LCD_DEFCHAR 		1		// <--- send custom character form RAM
#define USE_LCD_DEFCHAR_P 		1		// <--- send custom character form FLASH
#define USE_LCD_DEFCHAR_E		1		// <--- send custom character form EEPROM

#define USE_LCD_SHIFTDISPLAY	1		// <--- LCD content shift right/left

#define USE_LCD_CURSOR_ON 		0		// <--- cursor on/off
#define USE_LCD_CURSOR_BLINK	0		// <--- cursor blinking on/off
#define USE_LCD_CURSOR_HOME 	1		// <--- set cursor at home position

//*****************************************************************************************



//====================================================
//													 |
//		CONTROL LINES SETTINGS						 |
//													 |
//====================================================

//------------------------------------------------
// LCD resolution (number of rows & columnts)
//------------------------------------------------
#define LCD_ROWS 2
#define LCD_COLS 16


//-----------------
// 	RW control
//-----------------
#define USE_RW 1


// ports & pins configuration of data lines D7-D4
#define LCD_D7PORT C
#define LCD_D7 7
#define LCD_D6PORT C
#define LCD_D6 6
#define LCD_D5PORT C
#define LCD_D5 5
#define LCD_D4PORT C
#define LCD_D4 4

// ports & pins configuration of control lines RS, RW, E
#define LCD_RSPORT C
#define LCD_RS 1
#define LCD_RWPORT C
#define LCD_RW 2
#define LCD_EPORT C
#define LCD_E 3

// ports & pins macros
// **** PORT
#define PORT(x) SPORT(x)
#define SPORT(x) (PORT##x)
// **** PIN
#define PIN(x) SPIN(x)
#define SPIN(x) (PIN##x)
// **** DDR
#define DDR(x) SDDR(x)
#define SDDR(x) (DDR##x)


//******************************************
//*			  CONTROL COMMANDS			   *
//******************************************
#define LCDC_CLS 			0x01		// clear screen
#define LCDC_HOME			0x02		// return home
#define LCDC_ENTRY			0x04		// shift cursor..
	#define LCDC_ENTRYR 		0x02		// ..right
	#define LCDC_ENTRYL			0			// ..left
	#define LCDC_MOVER			0x01		// ..right
	#define LCDC_MOVEL			0x03		// ..left
#define LCDC_ONOFF			0x08		// lcd on/off..
	#define LCDC_DISPLAYON		0x04		// ..on
	#define LCDC_CURSORON		0x02		// ..cursor on
	#define LCDC_CURSOROFF		0			// ..cursor off
	#define LCDC_BLINKON		0x01		// ..cursor blinking on
#define LCDC_SHIFT 			0x10		// content shift..
	#define LCDC_SHIFTDISP		0x08		// ..shift
	#define LCDC_SHIFTR			0x04		// ..right
	#define LCDC_SHIFTL			0			// ..left
#define LCDC_FUNC			0x20		// interface, number of lines & char pixels
	#define LCDC_FUNC8B			0x10		// 8-bit interface
	#define LCDC_FUNC4B			0			// 4-bit interface
	#define LCDC_FUNC2L			0x08		// 2 lines
	#define LCDC_FUNC1L			0			// 1 line
	#define LCDC_FUNC5x10		0x04		// 5x10 pixels
	#define LCDC_FUNC5x7		0			// 5x7 pixels
#define LCDC_SET_CGRAM		0x40
#define LCDC_SET_DDRAM		0x80



// function declarations
void lcd_init(void);
void lcd_cls(void);
void lcd_str(char * str);

void lcd_locate(int8_t y, int8_t x);

void lcd_char(char c);
void lcd_str_P(const char * str);
void lcd_str_E(char * str);
void lcd_int(int val);
void lcd_hex(uint32_t val);
void lcd_defchar(uint8_t nr, uint8_t *def_znak);
void lcd_defchar_P(uint8_t nr, const uint8_t *def_znak);
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak);

void lcd_home(void);
void lcd_cursor_on(void);
void lcd_cursor_off(void);
void lcd_blink_on(void);
void lcd_blink_off(void);


// DDRAM address definition for different types of LCDs
// 4x20 LCD
#if ( (LCD_ROWS == 4 ) && (LCD_COLS == 20) )
#define LCD_LINE1 0x00	//char 1, row 0
#define LCD_LINE2 0x28	//char 1, row 1
#define LCD_LINE3 0x14	//char 1, row 2
#define LCD_LINE4 0x54	//char 1, row 3
#else	// 2x16 LCD
#define LCD_LINE1 0x00	//char 1, row 0
#define LCD_LINE2 0x40	//char 1, row 1
#define LCD_LINE3 0x10	//char 1, row 2
#define LCD_LINE4 0x50	//char 1, row 3
#endif

#endif /* LCD_LCD_LIB_H_ */
