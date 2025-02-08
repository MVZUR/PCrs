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


//makrodefinicje operacji na sygna³ach RS, RW i E
#define SET_RS PORT(LCD_RSPORT) |= (1<<LCD_RS)		//STAN WYSOKI NA RS
#define CLR_RS PORT(LCD_RSPORT) &= ~(1<<LCD_RS)		//stan niski na RS

#define SET_RW PORT(LCD_RWPORT) |= (1<<LCD_RW)		//STAN WYSOKI NA RW
#define CLR_RW PORT(LCD_RWPORT) &= ~(1<<LCD_RW)		//stan niski na RW

#define SET_E PORT(LCD_EPORT) |= (1<<LCD_E)			//STAN WYSOKI NA E
#define CLR_E PORT(LCD_EPORT) &= ~(1<<LCD_E)		//stan niski na E


//----------------------------------
// deklaracja funkcji BusyFlag
//----------------------------------

uint8_t check_BF(void);


//------------------------------------------------------
// funkcja ustawiaj¹ca piny portów D7-D4 jako wyjœcia
//------------------------------------------------------
static inline void data_dir_out(void)
{
	DDR(LCD_D7PORT) |= (1<<LCD_D7);
	DDR(LCD_D6PORT) |= (1<<LCD_D6);
	DDR(LCD_D5PORT) |= (1<<LCD_D5);
	DDR(LCD_D4PORT) |= (1<<LCD_D4);
}


//---------------------------------------------------
// funkcja ustawiaj¹ca piny portów D7-D4 jako wejœcia
//---------------------------------------------------
static inline void data_dir_in(void)
{
	DDR(LCD_D7PORT) &= ~(1<<LCD_D7);
	DDR(LCD_D6PORT) &= ~(1<<LCD_D6);
	DDR(LCD_D5PORT) &= ~(1<<LCD_D5);
	DDR(LCD_D4PORT) &= ~(1<<LCD_D4);
}



//*********************************
//
// ZAPIS DANYCH DO WYŒWIETLACZA
//
//*********************************

//------------------------------------------
// zapis po³ówki bajtu do wyœwietlacza LCD
//------------------------------------------
static inline void lcd_sendHalf(uint8_t data)			//ta funkcja przesy³a tylko 4 m³odsze bity
{														//bo patrzy na data, która jest na bitach 0-3)

	if (data&(1<<0)) PORT(LCD_D4PORT) |= (1<<LCD_D4); else PORT(LCD_D4PORT) &= ~(1<<LCD_D4);
	if (data&(1<<1)) PORT(LCD_D5PORT) |= (1<<LCD_D5); else PORT(LCD_D5PORT) &= ~(1<<LCD_D5);
	if (data&(1<<2)) PORT(LCD_D6PORT) |= (1<<LCD_D6); else PORT(LCD_D6PORT) &= ~(1<<LCD_D6);
	if (data&(1<<3)) PORT(LCD_D7PORT) |= (1<<LCD_D7); else PORT(LCD_D7PORT) &= ~(1<<LCD_D7);
}

//-------------------------------------------------
// odczyt po³ówki bajtu z wyœwietlacza LCD
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
// zapis ca³ego bajtu do wyœwietlacza LCD
//-----------------------------------------------
void _lcd_write_byte(unsigned char _data)
{
	//ustawienie pinów portów D7-D4 jako wyjœcia
	data_dir_out();

#if USE_RW == 1
	CLR_RW;
#endif
	SET_E;
	//wysy³anie starszej czêœci bajtu danych D7-D4
	lcd_sendHalf(_data >> 4);		//(data odczytuje tylko m³odsz¹ czêœæ bitów
								//wiêc trzeba przepchaæ starsz¹ czêœæ do m³odszej
								//i teraz lcd_sendHalf zwróci _data z 4 starszymi bitami)
	CLR_E;

	SET_E;
	lcd_sendHalf(_data);		//wysy³anie m³odszej czêœci bajtu danych D3-D0
	CLR_E;

//sprawdzanie zawartoœci Busy Flag
#if USE_RW == 1
	while( (check_BF() & (1<<7)) );
#else
	_delay_us(120);
#endif
}

//-------------------------------------------------
// odczyt ca³ego bajtu z wyœwietlacza LCD
//-------------------------------------------------
#if USE_RW == 1
uint8_t _lcd_read_byte(void)
{
	uint8_t result = 0;
	data_dir_in();

	SET_RW;
	SET_E;
	//odczyt starszej czêœci bajtu danych D7-D4
	result |= (lcd_readHalf() << 4);	//(result w funkcji wy¿ej zaczyna siê od 0 wiêc trzeba
										//przepchaæ ten result do starszej czêœci czyli o 4 w lewo)
	CLR_E;

	SET_E;
	//odczyt m³odszej czêœci bajtu danych D3-D0
	result |= lcd_readHalf();
	CLR_E;

	return result;
}
#endif


//-------------------------------
// Definicja funkcji BusyFlag
//-------------------------------
#if USE_RW == 1
uint8_t check_BF(void)
{
	CLR_RS;
	return _lcd_read_byte();
}
#endif


//-------------------------------------------
// zapis komendy do wyœwietlacza LCD
//-------------------------------------------
void lcd_write_cmd(uint8_t cmd)
{
	CLR_RS;			//aby wys³aæ komendê do wyœwietlacza na RS musi byæ 0
	_lcd_write_byte(cmd);
}


//----------------------------------------
// zapis danych do wyœwietlacza LCD
//----------------------------------------
void lcd_write_data(uint8_t data)
{
	SET_RS;			//aby wys³aæ dane do wyœwietlacza na RS musi byæ 1
	_lcd_write_byte(data);
}



//**************** funkcje przeznaczone tak¿e dla innych modu³ów ******************


//----------------------------------------------------------------------------------------
//
//		 Wysy³anie pojedyñczego znaku w postaci argumentu
//
//		 8 w³asnych znaków zdefiniowanych w CGRAM
//		 wysy³amy za pomoc¹ kodów 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
#if USE_LCD_CHAR == 1
void lcd_char(char c)
{
//	if( lcd_y>=0 && lcd_y<LCD_ROWS && lcd_x>=0 && lcd_x<LCD_COLS)
//	{
//		lcd_locate(lcd_y,lcd_x);
		lcd_write_data( ( c>=0x80 && c<=0x87 ) ? (c & 0x07) : c);
//	}
//	lcd_x++;
}
#endif


//--------------------------------------------------------------
//		 Wys³anie stringa do wyœwietlacza LCD z pamiêci RAM
//--------------------------------------------------------------
void lcd_str(char *str)
{
	register char znak;
	while ( (znak=*(str++)) ) lcd_char( znak );
}



//-------------------------------------------
// Wysy³anie stringa z pamiêci FLASH
//-------------------------------------------
#if USE_LCD_STR_P == 1
void lcd_str_P(const char *str)
{
	register char znak;
	while ( (znak=pgm_read_byte(str++)) ) lcd_char( znak );
}
#endif


//-------------------------------------
// Wysy³anie stringa z pamiêci EEPROM
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
// Wyœwietlanie liczby dziesiêtnej na LCD
//----------------------------------------
#if USE_LCD_INT == 1
void lcd_int(int val)
{
	char bufor[17];
	lcd_str( itoa(val, bufor, 10) );
}
#endif


//----------------------------------------------
// Wyœwietlanie liczby heksadecymalnej na LCD
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
//		Definicja w³asnego znaku na LCD z pamiêci RAM
//
//		argumenty:
//		nr: 		- kod znaku w pamiêci CGRAM od 0x80 do 0x87
//		*def_znak:	- wskaŸnik do tablicy 7 bajtów definiuj¹cych znak
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
// Definicja w³asnego znaku na LCD z pamiêci FLASH
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
// Definicja w³asnego znaku na LCD z pamiêci EEPROM
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
//		Ustawienie kursora w pozycji Y-wiersz, X-kolumna
//
// 		Y = od 0 do 3
// 		X = od 0 do n
//
//		funkcja dostosowuje automatycznie adresy DDRAM
//		w zale¿noœci od rodzaju wyœwietlacza (ile posiada wierszy)
//
//----------------------------------------------------------------------------------------
#if USE_LCD_LOCATE == 1
void lcd_locate(int8_t y, int8_t x)
{

	switch(y)
	{
		case 0: y = LCD_LINE1; break;	//adres 1 znaku wiersza 0

#if (LCD_ROWS>1)
		case 1: y = LCD_LINE2; break;	//adres 1 znaku wiersza 1
#endif

#if (LCD_ROWS>2)
		case 2: y = LCD_LINE3; break;	//adres 1 znaku wiersza 2
#endif

#if (LCD_ROWS>3)
		case 3: y = LCD_LINE4; break;	//adres 1 znaku wiersza 3
#endif
	}
	lcd_write_cmd( (0x80 + y + x) );
}
#endif



//=============================================|
//	 										   |
//	   FUNCKJE DO WYŒWIETLANIA TEKSTÓW		   |
//   		\/		  \/		\/			   |
//=============================================|

//----------------------------------------------------------------------------------------
//
//		Kasowanie ekranu wyœwietlacza
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
//		Powrót kursora na pocz¹tek
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
//		W³¹czenie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_cursor_on(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON);
}

//----------------------------------------------------------------------------------------
//
//		Wy³¹czenie kursora na LCD
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
//		W£¹cza miganie kursora na LCD
//
//----------------------------------------------------------------------------------------
void lcd_blink_on(void)
{
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON|LCDC_CURSORON|LCDC_BLINKON);
}

//----------------------------------------------------------------------------------------
//
//		WY³¹cza miganie kursora na LCD
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
//		Przesuwa ekran w prawo
//
//----------------------------------------------------------------------------------------
void lcd_shift_R(void)
{
	lcd_write_cmd( LCDC_SHIFT|LCDC_SHIFTDISP|LCDC_SHIFTR);
}
//----------------------------------------------------------------------------------------
//
//		Przesuwa ekran w lewo
//
//----------------------------------------------------------------------------------------
void lcd_shift_L(void)
{
	lcd_write_cmd( LCDC_SHIFT|LCDC_SHIFTDISP|LCDC_SHIFTL);
}
#endif




//*******************************************************
//***>>>>>>>> inicjalizacja wyœwietlacza LCD <<<<<<<<<***
//*******************************************************
//wed³ug algorytmu pracy w trybie 4-bitowym
void lcd_init(void)
{
	//ustawienie pinów D7-D4 jako wyjœcia
	data_dir_out();
	//ustawienie linii steruj¹cych RS, (if)RW, E jako wyjœcia
	DDR(LCD_RSPORT) |= (1<<LCD_RS);
	DDR(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
		DDR(LCD_RWPORT) |= (1<<LCD_RW);
	#endif
	//ustawienie stanu wysokiego na liniach steruj¹cych RS, (if)RW, E
	PORT(LCD_RSPORT) |= (1<<LCD_RS);
	PORT(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
	PORT(LCD_RWPORT) |= (1<<LCD_RW);
	#endif
	_delay_ms(15);		//odczekanie 15 ms
	//wyzerowanie linii steruj¹cych
	PORT(LCD_EPORT) &= ~(1<<LCD_E);
	PORT(LCD_RSPORT) &= ~(1<<LCD_RS);
	#if USE_RW == 1
		PORT(LCD_RWPORT) &= ~(1<<LCD_RW);
	#endif

	SET_E;
	lcd_sendHalf(0x03);		//tryb 8-bitowy D4 = 1
	CLR_E;
	_delay_ms(4.1);

	SET_E;
	lcd_sendHalf(0x03);		//tryb 8-bitowy D4 = 1
	CLR_E;
	_delay_us(100);

	SET_E;
	lcd_sendHalf(0x03);		//tryb 4-bitowy D4 = 1
	CLR_E;
	_delay_us(100);

	SET_E;
	lcd_sendHalf(0x02);		//tryb 4-bitowy D4 = 0	(ju¿ mo¿na u¿waæ BusyFlag)
	CLR_E;
	_delay_us(100);
	//u¿ywanie funkcji do wysy³ania komend:

	//tryb 4-bitowy, 2 wiersze, znak 5x7
	lcd_write_cmd( LCDC_FUNC|LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7 );
	//wy³¹czanie kursora
	lcd_write_cmd( LCDC_ONOFF|LCDC_CURSOROFF );
	//w³¹czanie wyœwietlacza
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON );
	//przesuwanie kursora w prawo bez przesuwania zawartoœci ekranu
	lcd_write_cmd( LCDC_ENTRY|LCDC_ENTRYR );
	//wyczyszczenie ekranu
	lcd_cls();
}

