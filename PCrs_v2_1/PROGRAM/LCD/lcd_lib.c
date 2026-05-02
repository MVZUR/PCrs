/*
 * lcd_lib.c
 *
 *  Created on: 30 sie 2019
 *      Author: Maksymilian
 */

#include "lcd_lib.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>


// RS, RW & E macros
#define SET_RS PORT(LCD_RSPORT) |= (1<<LCD_RS)		//RS hi
#define CLR_RS PORT(LCD_RSPORT) &= ~(1<<LCD_RS)		//RS lo

#define SET_RW PORT(LCD_RWPORT) |= (1<<LCD_RW)		//RW hi
#define CLR_RW PORT(LCD_RWPORT) &= ~(1<<LCD_RW)		//RW lo

#define SET_E PORT(LCD_EPORT) |= (1<<LCD_E)			//E hi
#define CLR_E PORT(LCD_EPORT) &= ~(1<<LCD_E)		//E lo


//---------------------
//		BusyFlag
//---------------------

uint8_t check_BF(void);


//------------------------------------------------------
// set D7-D4 as output
//------------------------------------------------------
static inline void data_dir_out(void)
{
	DDR(LCD_D7PORT) |= (1<<LCD_D7);
	DDR(LCD_D6PORT) |= (1<<LCD_D6);
	DDR(LCD_D5PORT) |= (1<<LCD_D5);
	DDR(LCD_D4PORT) |= (1<<LCD_D4);
}


//---------------------------------------------------
// set D7-D4 as input
//---------------------------------------------------
static inline void data_dir_in(void)
{
	DDR(LCD_D7PORT) &= ~(1<<LCD_D7);
	DDR(LCD_D6PORT) &= ~(1<<LCD_D6);
	DDR(LCD_D5PORT) &= ~(1<<LCD_D5);
	DDR(LCD_D4PORT) &= ~(1<<LCD_D4);
}



//*********************
//
// SAVE DATA TO DISPLAY
//
//*********************

//------------------------------------------
// write half of byte
//------------------------------------------
static inline void lcd_sendHalf(uint8_t data)			//4 less significant bits
{
	if (data&(1<<0)) PORT(LCD_D4PORT) |= (1<<LCD_D4); else PORT(LCD_D4PORT) &= ~(1<<LCD_D4);
	if (data&(1<<1)) PORT(LCD_D5PORT) |= (1<<LCD_D5); else PORT(LCD_D5PORT) &= ~(1<<LCD_D5);
	if (data&(1<<2)) PORT(LCD_D6PORT) |= (1<<LCD_D6); else PORT(LCD_D6PORT) &= ~(1<<LCD_D6);
	if (data&(1<<3)) PORT(LCD_D7PORT) |= (1<<LCD_D7); else PORT(LCD_D7PORT) &= ~(1<<LCD_D7);
}

//-------------------------------------------------
// read half of byte
//-------------------------------------------------
#if USE_RW == 1
static inline uint8_t lcd_readHalf(void)
{
	uint8_t result = 0;
	if (PIN(LCD_D4PORT) & (1<<LCD_D4)) result |= (1<<0);
	if (PIN(LCD_D5PORT) & (1<<LCD_D5)) result |= (1<<1);
	if (PIN(LCD_D6PORT) & (1<<LCD_D6)) result |= (1<<2);
	if (PIN(LCD_D7PORT) & (1<<LCD_D7)) result |= (1<<3);
	return result;
}
#endif



//-----------------------------------------------
// write full byte
//-----------------------------------------------
void _lcd_write_byte(unsigned char _data)
{
	data_dir_out();

#if USE_RW == 1
	CLR_RW;
#endif

	SET_E;
	lcd_sendHalf(_data >> 4);	// send 4 first most significant bits
	CLR_E;

	SET_E;
	lcd_sendHalf(_data);		// send 4 less significant bits
	CLR_E;

//check Busy Flag
#if USE_RW == 1
	while( (check_BF() & (1<<7)) );
#else
	_delay_us(120);
#endif
}

//-------------------------------------------------
// read full byte
//-------------------------------------------------
#if USE_RW == 1
uint8_t _lcd_read_byte(void)
{
	uint8_t result = 0;
	data_dir_in();

	SET_RW;

	SET_E;
	result |= (lcd_readHalf() << 4);	// read 4 first most significant bits
	CLR_E;

	SET_E;
	result |= lcd_readHalf();			// read 4 less significant bits
	CLR_E;

	return result;
}
#endif


//-------------------------------
//	BusyFlag definition
//-------------------------------
#if USE_RW == 1
uint8_t check_BF(void)
{
	CLR_RS;
	return _lcd_read_byte();
}
#endif


//-------------------------------------------
// 	write command to LCD
//-------------------------------------------
void lcd_write_cmd(uint8_t cmd)
{
	CLR_RS;
	_lcd_write_byte(cmd);
}


//----------------------------------------
// 	write data to LCD
//----------------------------------------
void lcd_write_data(uint8_t data)
{
	SET_RS;
	_lcd_write_byte(data);
}


//---------------------------------
//	write single character to LCD
//---------------------------------
#if USE_LCD_CHAR == 1
void lcd_char(char c)
{
	lcd_write_data( ( c>=0x80 && c<=0x87 ) ? (c & 0x07) : c);
}
#endif


//--------------------------------------------------------------
//	write string to LCD from RAM
//--------------------------------------------------------------
void lcd_str(char *str)
{
	register char znak;
	while ( (znak=*(str++)) ) lcd_char( znak );
}



//-------------------------------------------
// 	write string to LCD from FLASH
//-------------------------------------------
#if USE_LCD_STR_P == 1
void lcd_str_P(const char *str)
{
	register char znak;
	while ( (znak=pgm_read_byte(str++)) ) lcd_char( znak );
}
#endif


//-------------------------------------
//	write string to LCD from EEPROM
//-------------------------------------
#if USE_LCD_STR_E == 1
void lcd_str_E(char *str)
{
	register char znak;
	while(1)
	{
		znak=eeprom_read_byte( (uint8_t *)(str++) );
		if(!znak || znak == 0xFF) break;
		else lcd_char( znak );
	}
}
#endif

//----------------------------------------
// 	write int to LCD
//----------------------------------------
#if USE_LCD_INT == 1
void lcd_int(int val)
{
	char bufor[17];
	lcd_str( itoa(val, bufor, 10) );
}
#endif


//----------------------------------------------
//	write hex to LCD
//----------------------------------------------
#if USE_LCD_HEX == 1
void lcd_hex(uint32_t val)
{
	char bufor[17];
	lcd_str( ltoa(val, bufor, 16) );
}
#endif


//----------------------------------------------------------------------------------------
//
//		Custom character definition from RAM
//
//		arguments:
//		nr: 		- char code in CGRAM memory (from 0x80 to 0x87)
//		*def_znak:	- pointer to 7-bytes tab which define custom char
//
//----------------------------------------------------------------------------------------
#if USE_LCD_DEFCHAR == 1
void lcd_defchar(uint8_t nr, uint8_t *def_znak)
{
	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07) *8) );
	for(i=0;i<8;i++)
	{
		c = *(def_znak++);
		lcd_write_data(c);
	}
}
#endif


//---------------------------------------------------
//	Custom character definition from FLASH
//---------------------------------------------------
#if USE_LCD_DEFCHAR_P == 1
void lcd_defchar_P(uint8_t nr, const uint8_t *def_znak)
{
	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07) *8) );
	for(i=0;i<8;i++)
	{
		c = pgm_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
#endif


//---------------------------------------------------
// 	Custom character definition from EEPROM
//---------------------------------------------------
#if USE_LCD_DEFCHAR_E == 1
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak)
{
	register uint8_t i,c;

	lcd_write_cmd( 64+((nr&0x07) *8) );
	for(i=0;i<8;i++)
	{
		c = eeprom_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
#endif



//----------------------------------------------------------------------------------------
//
//		Set cursor in position (Y-row, X-column)
//
//		Function adjust DDRAM adress automatically
//		depending on display type (how many rows & columns))
//
//----------------------------------------------------------------------------------------
#if USE_LCD_LOCATE == 1
void lcd_locate(int8_t y, int8_t x)
{

	switch(y)
	{
		case 0: y = LCD_LINE1; break;	//adress of first char - row 0

#if (LCD_ROWS>1)
		case 1: y = LCD_LINE2; break;	//adress of first char - row 1
#endif

#if (LCD_ROWS>2)
		case 2: y = LCD_LINE3; break;	//adress of first char - row 2
#endif

#if (LCD_ROWS>3)
		case 3: y = LCD_LINE4; break;	//adress of first char - row 3
#endif
	}
	lcd_write_cmd( (0x80 + y + x) );
}
#endif



//=============================================|
//	 										   |
//	   	  FUNCTIONS TO DISPLAY TEXTS		   |
//   		\/		  \/		\/			   |
//=============================================|

//----------------------------------------------------------------------------------------
//
//		clear screen
//
//----------------------------------------------------------------------------------------
void lcd_cls(void)
{

	lcd_write_cmd( LCDC_CLS );

	#if USE_RW == 0
		_delay_ms(4.9);
	#endif
}


#if USE_LCD_CURSOR_HOME == 1
//----------------------------------------------------------------------------------------
//
//		cursor return to home position
//
//----------------------------------------------------------------------------------------
void lcd_home(void)
{
	lcd_write_cmd( LCDC_CLS|LCDC_HOME );

	#if USE_RW == 0
		_delay_ms(4.9);
	#endif
}
#endif

#if USE_LCD_CURSOR_ON == 1
//----------------------------------------------------------------------------------------
//
//		turn cursor on
//
//----------------------------------------------------------------------------------------
void lcd_cursor_on(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON);
}

//----------------------------------------------------------------------------------------
//
//		turn cursor off
//
//----------------------------------------------------------------------------------------
void lcd_cursor_off(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON);
}
#endif


#if USE_LCD_CURSOR_BLINK == 1
//----------------------------------------------------------------------------------------
//
//		turn cursor blinking on
//
//----------------------------------------------------------------------------------------
void lcd_blink_on(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON|LCDC_BLINKON);
}

//----------------------------------------------------------------------------------------
//
//		turn cursor blinking off
//
//----------------------------------------------------------------------------------------
void lcd_blink_off(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON);
}
#endif

#if USE_LCD_SHIFTDISPLAY == 1
//----------------------------------------------------------------------------------------
//
//		right shift screen content
//
//----------------------------------------------------------------------------------------
void lcd_shift_R(void)
{
	lcd_write_cmd( LCDC_SHIFT|LCDC_SHIFTDISP|LCDC_SHIFTR);
}
//----------------------------------------------------------------------------------------
//
//		left shift screen content
//
//----------------------------------------------------------------------------------------
void lcd_shift_L(void)
{
	lcd_write_cmd( LCDC_SHIFT|LCDC_SHIFTDISP|LCDC_SHIFTL);
}
#endif




//*******************************************
//***>>>>>>>> LCD initialization <<<<<<<<<***
//*******************************************

// 4-bit mode algorithm
void lcd_init(void)
{
	//set D7-D4 as output
	data_dir_out();
	//set RS, (if)RW, E as output
	DDR(LCD_RSPORT) |= (1<<LCD_RS);
	DDR(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
		DDR(LCD_RWPORT) |= (1<<LCD_RW);
	#endif
	//set hi RS, (if)RW, E
	PORT(LCD_RSPORT) |= (1<<LCD_RS);
	PORT(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
	PORT(LCD_RWPORT) |= (1<<LCD_RW);
	#endif
	_delay_ms(15);		//wait 15 ms
	//set lo RS, (if)RW, E
	PORT(LCD_EPORT) &= ~(1<<LCD_E);
	PORT(LCD_RSPORT) &= ~(1<<LCD_RS);
	#if USE_RW == 1
		PORT(LCD_RWPORT) &= ~(1<<LCD_RW);
	#endif

	SET_E;
	lcd_sendHalf(0x03);		//8-bit mode D4 = 1
	CLR_E;
	_delay_ms(4.1);

	SET_E;
	lcd_sendHalf(0x03);		//8-bit mode D4 = 1
	CLR_E;
	_delay_us(100);

	SET_E;
	lcd_sendHalf(0x03);		//4-bit mode D4 = 1
	CLR_E;
	_delay_us(100);

	SET_E;
	lcd_sendHalf(0x02);		//4-bit mode D4 = 0
	CLR_E;
	_delay_us(100);

	//sending commands

	//4-bit mode, 2 rows, single character size 5x7
	lcd_write_cmd( LCDC_FUNC|LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7 );
	//cursor off
	lcd_write_cmd( LCDC_ONOFF|LCDC_CURSOROFF );
	//display on
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON );
	// cursor shift without shifting display content
	lcd_write_cmd( LCDC_ENTRY|LCDC_ENTRYR );
	//lcd clear
	lcd_cls();
}

