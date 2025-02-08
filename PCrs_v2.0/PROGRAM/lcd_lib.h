/*
 * lcd_lib.h
 *
 *  Created on: 30 sie 2019
 *      Author: Maksymilian
 */

#ifndef LCD_LCD_LIB_H_
#define LCD_LCD_LIB_H_

#include <avr/eeprom.h>


//******************************************************
//													   *
//			U S T A W I E N I A			KOMPILACJI	   *
//													   *
//*****************************************************************************************

#define USE_LCD_LOCATE			1		// <--- ustawia kursor na wybranej pozycji Y, X

#define USE_LCD_CHAR 			1		// <--- wysy³a pojedynczy znak jako argument funkcji

#define USE_LCD_STR_P 			1		// <--- wysy³a string umieszczony w pamiêci FLASH
#define USE_LCD_STR_E 			1		// <--- wysy³a string umieszczony w pamiêci EEPROM

#define USE_LCD_INT 			1		// <--- wyœwietla liczbê dziesiêtn¹ na LCD
#define USE_LCD_HEX 			1		// <--- wyœwietla liczbê szesnastkow¹ na LCD

#define USE_LCD_DEFCHAR 		1		// <--- wyœwietla zdefiniowany znak w pamiêci RAM
#define USE_LCD_DEFCHAR_P 		1		// <--- wyœwietla zdefiniowany znak w pamiêci FLASH
#define USE_LCD_DEFCHAR_E		1		// <--- wyœwietla zdefiniowany znak w pamiêci EEPROM

#define USE_LCD_SHIFTDISPLAY	1		// <--- przesuwa ekran w prawo/lewo

#define USE_LCD_CURSOR_ON 		0		// <--- w³¹cz / wy³¹cz kursor
#define USE_LCD_CURSOR_BLINK	0		// <--- w³¹cz / wy³¹cz miganie kursora
#define USE_LCD_CURSOR_HOME 	1		// <--- ustawia kursor w pozycji pocz¹tkowej

//*****************************************************************************************



//====================================================
//													 |
//		USTAWIENIA LINII STERUJ¥CYCH (PO£¥CZEÑ)		 |
//													 |
//====================================================

//------------------------------------------------
// rozdzielczoœæ wyœwietlacza (kolumny, wiersze)
//------------------------------------------------
#define LCD_ROWS 2 	//liczba wierszy wyœwietlacza LCD
#define LCD_COLS 16	//liczba kolumn wyœwietlacza LCD


//-----------------
// sterowanie RW
//-----------------
//czy chcesz sterowaæ RW?
//0 - nie 1 - tak
#define USE_RW 1


// konfiguracja portu i pinów, do których s¹ pod³¹czone linie D7-D4 LCD
#define LCD_D7PORT C
#define LCD_D7 7
#define LCD_D6PORT C
#define LCD_D6 6
#define LCD_D5PORT C
#define LCD_D5 5
#define LCD_D4PORT C
#define LCD_D4 4

// konfiguracja portu i pinów, do których s¹ pod³¹czone sygna³y RS, RW, E
#define LCD_RSPORT C
#define LCD_RS 1
#define LCD_RWPORT C
#define LCD_RW 2
#define LCD_EPORT C
#define LCD_E 3

// makra uproszczaj¹ce dostêp do portów
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
//*										   *
//*			  Komendy Steruj¹ce			   *
//*	 LCDC - lcd commads		 			   *
//******************************************
#define LCDC_CLS 			0x01		//czyszczenie ekranu
#define LCDC_HOME			0x02		//powrót do HOME
#define LCDC_ENTRY			0x04		//przesuñ kursor (pisz¹c):
	#define LCDC_ENTRYR 		0x02		//w prawo
	#define LCDC_ENTRYL			0			//w lewo
	#define LCDC_MOVER			0x01		//w prawo (bez przesuwania)
	#define LCDC_MOVEL			0x03		//w lewo (bez przesuwania)
#define LCDC_ONOFF			0x08		//sterowanie wyœwietlaczem (w³¹cz/wy³¹cz)
	#define LCDC_DISPLAYON		0x04		//w³¹cz wyœwietlacz
	#define LCDC_CURSORON		0x02		//w³¹cz kursor
	#define LCDC_CURSOROFF		0			//wy³¹cz kursor
	#define LCDC_BLINKON		0x01		//w³¹cz miganie kursora
#define LCDC_SHIFT 			0x10		//przesuniêcie ekranu
	#define LCDC_SHIFTDISP		0x08		//przesuñ ekran:
	#define LCDC_SHIFTR			0x04		//przesuñ w prawo
	#define LCDC_SHIFTL			0			//przesuñ w lewo
#define LCDC_FUNC			0x20		//funkcja (jaki interfejs, ile linii i ile pikseli)
	#define LCDC_FUNC8B			0x10		//interfejs 8 bitowy
	#define LCDC_FUNC4B			0			//interfejs 4 bitowy
	#define LCDC_FUNC2L			0x08		//liczba linii interfejsu: 2
	#define LCDC_FUNC1L			0			//liczba linii interfejsu: 1
	#define LCDC_FUNC5x10		0x04		// 5x10 pixeli
	#define LCDC_FUNC5x7		0			// 5x7 pixeli
#define LCDC_SET_CGRAM		0x40
#define LCDC_SET_DDRAM		0x80



// deklaracje funkcji na potrzeby innych modu³ów
void lcd_init(void);								// W£¥CZONA na sta³e do kompilacji
void lcd_cls(void);									// W£¥CZONA na sta³e do kompilacji
void lcd_str(char * str);							// W£¥CZONA na sta³e do kompilacji

void lcd_locate(int8_t y, int8_t x);				// domyœlnie W£¥CZONA z kompilacji w pliku lcd.c

void lcd_char(char c);								// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_str_P(const char * str);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_str_E(char * str);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_int(int val);								// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_hex(uint32_t val);								// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_defchar(uint8_t nr, uint8_t *def_znak);	// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_defchar_P(uint8_t nr, const uint8_t *def_znak);	// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak);	// domyœlnie wy³¹czona z kompilacji w pliku lcd.c

void lcd_home(void);								// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_cursor_on(void);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_cursor_off(void);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_blink_on(void);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c
void lcd_blink_off(void);							// domyœlnie wy³¹czona z kompilacji w pliku lcd.c


// definicje adresów DDRAM dla ró¿nych wyœwietlaczy
// dla wyœw. 4x20
#if ( (LCD_ROWS == 4 ) && (LCD_COLS == 20) )
#define LCD_LINE1 0x00	//adres 1 znaku 1 wiersza
#define LCD_LINE2 0x28	//adres 1 znaku 2 wiersza
#define LCD_LINE3 0x14	//adres 1 znaku 3 wiersza
#define LCD_LINE4 0x54	//adres 1 znaku 4 wiersza
#else	//dla wyœwietlaczy 2x16
#define LCD_LINE1 0x00	//adres 1 znaku 1 wiersza
#define LCD_LINE2 0x40	//adres 1 znaku 2 wiersza
#define LCD_LINE3 0x10	//adres 1 znaku 3 wiersza
#define LCD_LINE4 0x50	//adres 1 znaku 4 wiersza
#endif

#endif /* LCD_LCD_LIB_H_ */
