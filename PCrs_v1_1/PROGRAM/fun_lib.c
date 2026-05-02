/*
 * fun_lib.c
 *
 *  Created on: 23 lip 2023
 *      Author: Lenovo
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "fun_lib.h"
#include "../LCD/lcd_lib.h"	//  ../ - wychodzi folder wy¿ej


//-----------------------
// 	SEKCJA USTAWIEÑ		|
//						|
//	USTAWIENIA DOMYŒLNE	|
//-------------------------------------------------------------------------------------
uint8_t use_buz = 0;	// zmienna wysy³aj¹ca informacjê do oprogramowania buzzera (x - iloœæ syg.)
						// x - buzzer aktyw. 	0 - buzzer dezaktyw.

uint8_t ref = 0;		// zmienna zapamiêtuj¹ca ustawienie trybu pracy reflektorów
						// 1 - MANUAL			0 - AUTO

uint8_t ref_set = 0;	// ustawienie reflektorów w trybie auto
						// 1 - mijania			0 - LEDY (dzienne)

uint8_t buz = 1;		// zmienna zapamiêtuj¹ca ustawienie buzzera
						// 1 - w³¹czony			0 - wy³¹czony
//-------------------------------------------------------------------------------------













// ******************************************************************************************
// *                                   STAN BEZCZYZNNOŒCI                                   *
// ******************************************************************************************

void idle_init(void)
{
	DDR(IDLE_PORT) &= ~IDLE_PIN;	// ustawienie pinu sygna³u wprowadzaj¹cego w stan idle (WEJŒCIE)
	PORT(IDLE_PORT) |= IDLE_PIN;	// podci¹gniêcie do VCC
}


void idle(void)
{
	if(IDLE_ACTIVE)
	{
		lcd_cls();				// wyczyœæ ekran


		TCCR2 &= ~(1<<CS22);	// wy³¹cz licznik RPM
		TCCR2 &= ~(1<<CS21);	// ...
		TCCR2 &= ~(1<<CS20);	// ...

		GICR &= ~(1<<INT0);		// zablokowanie przerwañ na pinie INT0

		ADCSRA &= ~(1<<ADEN);	// wy³¹czenie ADC



	}

	while(IDLE_ACTIVE)		// STAN BEZCZYNNOŒCI
	{
		blinkers_check();
		blinkers();
	}

		TCCR2 |= (1<<CS22);	// w³¹cz licznik RPM
		TCCR2 |= (1<<CS21);	// ...
		TCCR2 |= (1<<CS20);	// ...

		GICR |= (1<<INT0);		// odblokowanie przerwañ na pinie INT0

		ADCSRA |= (1<<ADEN);	// w³¹czenie ADC
}
// ******************************************************************************************















// ****************************************************************************************
// *                                      czujnik RPM                                     *
// ****************************************************************************************

uint8_t spark_sig = 1;					// sygna³ podawany z iskry
uint8_t check_spark_sig = 0;			// sprawdzanie czy sygna³ z iskry jest wysy³any
uint32_t delay_between_sparks = 0;		// odcinek czasu miêdzy iskrami
uint32_t rpm = 0;						// wartoœæ obrotów na minutê
uint8_t graph_value = 0;				// podzia³ka RPM na 16 wyœwietlanych graficznie prostok¹tów
uint8_t frequency_cutter2 = 0;			// zmienna zmniejszaj¹ca czêstotliwoœæ aktualizacji RPM
										// ... aby zapobiec "smu¿eniu"


void rpm_init(void)
{
	DDR(RPM_PORT) &= ~RPM_PIN;		// ustawienie pinu do pomiaru RPM (WEJŒCIE)
	PORT(RPM_PORT) &= ~RPM_PIN;		// podci¹gniêcie do GND
	TIMSK |= (1<<TOIE2);			// zezwolenie na przerwania TIMER2

	GICR |= (1<<INT0);				// odblokowanie przerwañ na pinie INT0

	MCUCR |= (1<<ISC01);			// wyzwolenie przerwania na zboczu opadaj¹cym
	MCUCR &= ~(1<<ISC00);			// ...
}


// procedura obs³ugi przerwania wyzwolonego zmian¹ stanu na pinie INT0
ISR(INT0_vect)
{
	spark_sig++;

	if(frequency_cutter2 > 10)
	{
		frequency_cutter2 = 0;
	}

	TCCR2 |= (1<<CS20);

	if(spark_sig > 50)			// odcinek czasu mierzony pomiêdzy 50-cioma iskrami
	{
		spark_sig = 0;


		TCCR2 &= ~(1<<CS22);	// wy³¹cz licznik buzzera
		TCCR2 &= ~(1<<CS21);	// ...
		TCCR2 &= ~(1<<CS20);	// ...

	}

	if(delay_between_sparks == 0)
	{
		_delay_ms(1);			// opóŸnienie potrzebne aby prawid³owo wykonaæ pomiary
	}

	else
	{
		if((3570000/delay_between_sparks) < 8000)
		{
			if(frequency_cutter2 == 0)
			{
				rpm = 3570000/delay_between_sparks;		// konwertowanie d³ugoœci odcinka czasu
			}											// .. na wartoœæ RPM

		}
		delay_between_sparks = 0;
	}

	frequency_cutter2++;
}


// procedura obs³ugi przerwania dla TIMER2
ISR(TIMER2_OVF_vect)
{
	delay_between_sparks++;						// <-- pomiar odcinka czasu miêdzy iskrami
}
// *****************************************************************************************















// *****************************************************************************************
// *                                         ZEGAR                                         *
// *****************************************************************************************

// --------------------
// NASTAWIANIE ZEGARA |
// --------------------
uint8_t sec = 0;				// <-- sekunda
uint8_t min = 58;				// <-- minuta
uint8_t hour = 18;				// <-- godzina
uint8_t day = 28;				// <-- dzieñ
uint8_t day_name = 5;			// <-- 1-pon / 2-wto / 3-œro / 4-czw / 5-pt / 6-sb / 7-nd
uint8_t month = 7;				// <-- miesi¹c
uint8_t feb_correction = 3;		// <-- rok przestêpny w Lutym
uint16_t year = 2023;			// <-- rok
// --------------------

// zmienne operacyjne
char dot = 58;					// migaj¹cy dwukropek

// rêczne ustawianie zegara
uint8_t mark1 = 0;
uint8_t mark2 = 0;
uint8_t clock_set = 0;


void clock_init(void)
{
	// inicjalizacja TIMER1
	TCCR1B |= (1<<WGM12);	// wybór trybu pracy CTC
	TCCR1B |= (1<<CS12);	// preskaler 256
	OCR1A = 31249;		//rejestr porównania
	TIMSK |= (1<<OCIE1A);	//zezwolenie na przerwanie
}


// procedura obs³ugi przerwania dla TIMER1 -> ZEGAR
ISR(TIMER1_COMPA_vect)
{
	sec++;

	if(dot == 58)
	{
		dot = 32;
	}
	else
	{
		dot = 58;
	}

	if(sec > 59)
	{
		min++;
		sec = 0;
	}

	if(min > 59)
	{
		hour++;
		min = 0;
	}

	if(hour > 23)
	{
		day++;
		hour = 0;

		day_name++;

		if(day_name > 7)
		{
			day_name = 1;
		}
	}

	if(month == 2)						// <-- luty
	{
		if(feb_correction == 4)			// <<< rok przestêpny
		{
			if(day > 29)
			{
				month++;
				day = 1;
			}
		}
		else if(day > 28)
		{
			month++;
			day = 1;
		}
	}
	else if((month == 1) || (month == 3) || (month == 5) || (month == 7) || (month == 8) || (month == 10) || (month == 12))
	{
		if(day > 31)
		{
			month++;
			day = 1;
		}
	}
	else
	{
		if(day > 30)
		{
			month++;
			day = 1;
		}
	}

	if(month > 12)
	{
		month = 1;
		year++;
		feb_correction++;
	}

	if(feb_correction == 4)
	{
		feb_correction = 0;
	}



	// DO WSKAZAÑ RPM:
	// sprawdzanie czy sygna³ z iskrownika jest wysy³any

	if(spark_sig == check_spark_sig)	// jeœli sygna³ z iskrownika nie zmieni³ siê przez sekundê
	{									// ...
		rpm = 0;						// wyzeruj wskazania
	}

	spark_sig = check_spark_sig;



	// ---------- RÊCZNE NASAWIANIE ZEGARA -----------------------------------------------

	if(LCD_SET_KEY_DOWN)
	{
		mark1++;		// odlicz do 3
	}
	else
	{
		mark1 = 0;		// jeœli SETkey zosta³ puszczony, wyzeruj
	}

	if(mark1 == 3)		// jeœli SETkey dalej wciœniêty, uruchom nastawianie zegara
	{
		PORT(BUZ_PORT) &= ~BUZ_PIN;

		// przyspiesz Timer1
		TCCR1B &= ~(1<<CS12);	// preskaler 64
		TCCR1B |= (1<<CS11);	// ...
		TCCR1B |= (1<<CS10);	// ...


		clock_set = 1;			// PROCEDURA NASTAWIANIA ZEGARA

		while(clock_set == 1)
		{

			lcd_locate(0,13);	// nawet jeœli kropki pomiêdzy godz. i min. zniknê³y,
			lcd_str(":");		// wyœwietl je ponownie

			_delay_ms(100);		// na spokojnie..

			// NAZWA DNIA ***********************
			lcd_locate(0,0);
			lcd_str("           ");
			_delay_ms(1000);
			lcd_locate(0,0);
			switch(day_name)	// wyœwietlanie
			{
				case 1:
					lcd_str("Monday");
					break;
				case 2:
					lcd_str("Tuesday");
					break;
				case 3:
					lcd_str("Wednesday");
					break;
				case 4:
					lcd_str("Thursday");
					break;
				case 5:
					lcd_str("Friday");
					break;
				case 6:
					lcd_str("Saturday");
					break;
				case 7:
					lcd_str("Sunday");
					break;
			}

			while(1) // <-- zmiana nazwy dnia
			{
				if(LCD_DISP_KEY_DOWN)	// SETkey - zwiêksz numer przypisany do nazwy dnia o 1
				{
					day_name++;


					if(day_name > 7)
					{
						day_name = 1;
					}

					PORT(BUZ_PORT) |= BUZ_PIN;	// sygna³ dŸwiêkowy

					lcd_locate(0,0);
					lcd_str("           ");		// <-- wskazanie co zostaje teraz zmieniane
					lcd_locate(0,0);
					switch(day_name)	// wyœwietlanie nazwy dnia
					{
						case 1:
							lcd_str("Monday");
							break;
						case 2:
							lcd_str("Tuesday");
							break;
						case 3:
							lcd_str("Wednesday");
							break;
						case 4:
							lcd_str("Thursday");
							break;
						case 5:
							lcd_str("Friday");
							break;
						case 6:
							lcd_str("Saturday");
							break;
						case 7:
							lcd_str("Sunday");
							break;
					}

					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;	// wy³. sygna³ dŸwiêkowy
					_delay_ms(280);
				}

				if(LCD_SET_KEY_DOWN)
				{
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(80);
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(380);

					if(LCD_SET_KEY_DOWN)
					{
						_delay_ms(1500);

						if(LCD_SET_KEY_DOWN)
						{
							PORT(BUZ_PORT) |= BUZ_PIN;
							_delay_ms(500);
							PORT(BUZ_PORT) &= ~BUZ_PIN;

							clock_set = 0;
							break;
						}
						else
						{
							break;
						}

					}
					else
					{
						break;
					}
				}
			}

			if(clock_set == 0)
			{
				break;
			}




			// GODZINY ***********************
			lcd_locate(0,11);
			lcd_str("  ");
			_delay_ms(1000);

			lcd_locate(0,11);
			if(hour < 10)		// wyœwietlanie
			{
				lcd_int(0);
				lcd_locate(0,12);
				lcd_int(hour);
			}
			else
			{
				lcd_int(hour);
			}


			while(1) // <-- zmiana godziny
			{
				if(LCD_DISP_KEY_DOWN) // SETkey - zwiêksz godzinê o 1
				{
					hour++;

					if(hour > 23)
					{
						hour = 0;
					}

					PORT(BUZ_PORT) |= BUZ_PIN;


					lcd_locate(0,11);

					if(hour < 10)		// wyœwietlanie
					{
						lcd_int(0);
						lcd_locate(0,12);
						lcd_int(hour);
					}
					else
					{
						lcd_int(hour);
					}

					_delay_ms(20);

					PORT(BUZ_PORT) &= ~BUZ_PIN;

					_delay_ms(280);
				}

				if(LCD_SET_KEY_DOWN)
				{
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(80);
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(380);

					if(LCD_SET_KEY_DOWN)
					{
						_delay_ms(1500);

						if(LCD_SET_KEY_DOWN)
						{
							PORT(BUZ_PORT) |= BUZ_PIN;
							_delay_ms(500);
							PORT(BUZ_PORT) &= ~BUZ_PIN;

							clock_set = 0;
							break;
						}
						else
						{
							break;
						}

					}
					else
					{
						break;
					}
				}
			}

			if(clock_set == 0)
			{
				break;
			}




			// MINUTY ***********************
			lcd_locate(0,14);
			lcd_str("  ");
			_delay_ms(1000);

			lcd_locate(0,14);
			if(min < 10)		// wyœwietlanie
			{
				lcd_int(0);
				lcd_locate(0,15);
				lcd_int(min);
			}
			else
			{

				lcd_int(min);
			}

			while(1)
			{
				if(LCD_DISP_KEY_DOWN)
				{
					min++;

					if(min > 59)
					{
						min = 0;
					}

					PORT(BUZ_PORT) |= BUZ_PIN;


					lcd_locate(0,14);

					if(min < 10)		// wyœwietlanie
					{
						lcd_int(0);
						lcd_locate(0,15);
						lcd_int(min);
					}
					else
					{

						lcd_int(min);
					}

					_delay_ms(20);

					PORT(BUZ_PORT) &= ~BUZ_PIN;

					_delay_ms(280);
				}

				if(LCD_SET_KEY_DOWN)
				{
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(80);
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(380);

					if(LCD_SET_KEY_DOWN)
					{
						_delay_ms(1500);

						if(LCD_SET_KEY_DOWN)
						{
							PORT(BUZ_PORT) |= BUZ_PIN;
							_delay_ms(500);
							PORT(BUZ_PORT) &= ~BUZ_PIN;

							clock_set = 0;
							break;
						}
						else
						{
							break;
						}

					}
					else
					{
						break;
					}
				}
			}

			if(clock_set == 0)
			{
				break;
			}



			// DNI ***********************
			lcd_locate(1,0);
			lcd_str("  ");
			_delay_ms(1000);


			lcd_locate(1,0);
			if(day < 10)		// wyœwietlanie
			{
				lcd_int(0);
				lcd_locate(1,1);
				lcd_int(day);
			}

			else
			{
				lcd_int(day);
			}

			while(1)
			{
				if(LCD_DISP_KEY_DOWN)
				{
					day++;

					if(month == 2)						// <-- luty
					{
						if(feb_correction == 4)			// <<< rok przestêpny
						{
							if(day > 29)
							{
								day = 1;
							}
						}
						else if(day > 28)
						{
							day = 1;
						}
					}
					else if((month == 1) || (month == 3) || (month == 5) || (month == 7) || (month == 8) || (month == 10) || (month == 12))
					{
						if(day > 31)
						{
							day = 1;
						}
					}
					else
					{
						if(day > 30)
						{
							day = 1;
						}
					}

					PORT(BUZ_PORT) |= BUZ_PIN;


					lcd_locate(1,0);
					if(day < 10)		// wyœwietlanie
					{
						lcd_int(0);
						lcd_locate(1,1);
						lcd_int(day);
					}

					else
					{
						lcd_int(day);
					}

					_delay_ms(20);

					PORT(BUZ_PORT) &= ~BUZ_PIN;

					_delay_ms(280);
				}

				if(LCD_SET_KEY_DOWN)
				{
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(80);
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(380);

					if(LCD_SET_KEY_DOWN)
					{
						_delay_ms(1500);

						if(LCD_SET_KEY_DOWN)
						{
							PORT(BUZ_PORT) |= BUZ_PIN;
							_delay_ms(500);
							PORT(BUZ_PORT) &= ~BUZ_PIN;

							clock_set = 0;
							break;
						}
						else
						{
							break;
						}

					}
					else
					{
						break;
					}
				}
			}

			if(clock_set == 0)
			{
				break;
			}



			// MIESI¥CE ***********************
			lcd_locate(1,3);
			lcd_str("  ");
			_delay_ms(1000);


			lcd_locate(1,3);
			if(month < 10)			// wyœwietlanie
			{
				lcd_int(0);
				lcd_locate(1,4);
				lcd_int(month);
			}

			else
			{
				lcd_int(month);
			}

			while(1)
			{
				if(LCD_DISP_KEY_DOWN)
				{
					month++;

					if(month > 12)
					{
						month = 1;
					}

					PORT(BUZ_PORT) |= BUZ_PIN;


					lcd_locate(1,3);
					if(month < 10)			// wyœwietlanie
					{
						lcd_int(0);
						lcd_locate(1,4);
						lcd_int(month);
					}

					else
					{
						lcd_int(month);
					}

					_delay_ms(20);

					PORT(BUZ_PORT) &= ~BUZ_PIN;

					_delay_ms(280);
				}

				if(LCD_SET_KEY_DOWN)
				{
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(80);
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(380);

					if(LCD_SET_KEY_DOWN)
					{
						_delay_ms(1500);

						if(LCD_SET_KEY_DOWN)
						{
							PORT(BUZ_PORT) |= BUZ_PIN;
							_delay_ms(500);
							PORT(BUZ_PORT) &= ~BUZ_PIN;

							clock_set = 0;
							break;
						}
						else
						{
							break;
						}

					}
					else
					{
						break;
					}
				}
			}

			if(clock_set == 0)
			{
				break;
			}



			// LATA ***********************
			lcd_locate(1,6);
			lcd_str("    ");
			_delay_ms(1000);


			lcd_locate(1,6);	//wyœwietlanie
			lcd_int(year);

			while(1)
			{
				if(LCD_DISP_KEY_DOWN)
				{
					year++;
					feb_correction++;

					if(feb_correction == 4)
					{
						feb_correction = 0;
					}

					if(year > 2050)
					{
						year = 2023;
						feb_correction = 3;
					}

					PORT(BUZ_PORT) |= BUZ_PIN;


					lcd_locate(1,6);	//wyœwietlanie
					lcd_int(year);

					_delay_ms(20);

					PORT(BUZ_PORT) &= ~BUZ_PIN;

					_delay_ms(280);
				}

				if(LCD_SET_KEY_DOWN)
				{
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(80);
					PORT(BUZ_PORT) |= BUZ_PIN;
					_delay_ms(20);
					PORT(BUZ_PORT) &= ~BUZ_PIN;
					_delay_ms(380);

					if(LCD_SET_KEY_DOWN)
					{
						_delay_ms(1500);

						if(LCD_SET_KEY_DOWN)
						{
							PORT(BUZ_PORT) |= BUZ_PIN;
							_delay_ms(500);
							PORT(BUZ_PORT) &= ~BUZ_PIN;

							clock_set = 0;
							break;
						}
						else
						{
							break;
						}

					}
					else
					{
						break;
					}
				}
			}

			if(clock_set == 0)
			{
				break;
			}

		}

		TCCR1B |= (1<<CS12);	// preskaler 256
		TCCR1B &= ~(1<<CS11);	// ...
		TCCR1B &= ~(1<<CS10);	// ...

		mark1 = 0;

	}
}
// *****************************************************************************************















// *****************************************************************************************
// *                                        CZUJNIKI                                       *
// *****************************************************************************************

uint8_t tmp = 0;
uint16_t tmp_value = 0;
uint16_t foto_value = 0;

uint8_t remember1 = 99;		// zmienne zapamiêtuj¹ca poprzedni stan ustawienia ref
uint8_t remember2 = 99;		// w celu unikniêcia powtórek

uint8_t frequency_cutter = 0;



void sensor_init(void)
{
	// inicjalizacja ADC
	ADMUX |= (1<<REFS1) | (1<<REFS0);			// wewnêtrzne Ÿród³o odniesienia (2,56V)
	ADCSRA |= (1<<ADEN);						// uruchomienie adc
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1);			// prescaller 64
}


void tmp_check(void)
{
	// KANA£ ADC0
	ADMUX = (ADMUX & 0xF8) | 0;

	ADCSRA |= (1<<ADSC);			// start pomiarów
	while( ADCSRA & (1<<ADSC) );	// oczekiwanie na koniec pomiarów

	tmp_value = (ADCW);				// wartoœæ odczytana przez ADC
	
	if(frequency_cutter > 200)		// aktualizuj wartoœæ co 200 odœwie¿eñ
	{								// aby zapobiec "smu¿eniu"
		frequency_cutter = 0;
	}

	if(frequency_cutter == 0)		// ...
	{
		// konwertowanie wartoœci z ADC na temperaturê
		tmp = (105 - ((0.09)*tmp_value));
	}

	frequency_cutter++;
}


void foto_sig_init(void)
{
	DDR(FOTO_SIG_MODE_PORT) |= FOTO_SIG_MODE_PIN;	// ustawienie pinu sygna³u AUTO-MANUAL (WYJŒCIE)
	DDR(FOTO_SIG_LIGHT_PORT) |= FOTO_SIG_LIGHT_PIN;	// ustawienie pinu sygna³u LED-drogowe (WYJŒCIE)
}


void foto_check(void)
{
	// KANA£ ADC1
	ADMUX = (ADMUX & 0xF8) | 1;

	ADCSRA |= (1<<ADSC);			// start pomiarów
	while( ADCSRA & (1<<ADSC) );	// oczekiwanie na koniec pomiarów

	foto_value = (ADCW);			// wartoœæ odczytana przez ADC


	if((ref == 0) && !(ref == remember1))		// tryb AUTO + uniknij powtórek
	{
		DDR(FOTO_SIG_MODE_PORT) &= ~FOTO_SIG_MODE_PIN;	// przekaŸnik w trybie AUTO

		remember1 = ref;
	}

	else if((ref == 1) && !(ref == remember1))
	{
		DDR(FOTO_SIG_MODE_PORT) |= FOTO_SIG_MODE_PIN;	// przekaŸnik w trybie MANUAL

		remember1 = ref;
	}

	if(ref == 0)					// tryb pracy reflektorów - AUTO
	{
		if((foto_value > 100) && !(remember2 == 1) )
		{
			DDR(FOTO_SIG_LIGHT_PORT) |= FOTO_SIG_LIGHT_PIN;	// œwiat³a mijania

			remember2 = 1;
		}

		else if((foto_value <= 100) && !(remember2 == 0))
		{
			DDR(FOTO_SIG_LIGHT_PORT) &= ~FOTO_SIG_LIGHT_PIN;	// dzienne LED

			remember2 = 0;
		}
	}
}
// *****************************************************************************************















// *****************************************************************************************
// *                                     STOPKA BOCZNA                                     *
// *****************************************************************************************

uint8_t check = 0;	// zmienna zapamiêtuj¹ca czy stan czujnika zosta³ sprawdzony
					// aby unikn¹æ powtórnego wyœwietlania tej samej wartoœci


void stand_init(void)	// inicjalizacja przycisku stopki bocznej
{
	DDR(STAND_KEY_PORT) &= ~STAND_KEY_PIN;	// ustawienie pinu przycisku stopki bocznej (WEJŒCIE)
	PORT(STAND_KEY_PORT) |= STAND_KEY_PIN;	// podci¹gniêcie do VCC
}


void stand_check(void)	// status ustawienia przycisku stopki bocznej
{
	check_again:

	if(STAND_KEY_DOWN && !(check == 1))	// czujnik wy³¹czony
	{
		check = 1;
		lcd_cls();
		lcd_str("stand sensor OFF");
		use_buz = 0;

		for(uint8_t i=0; i<200; i++)
		{
			if((buz == 1) && ((i == 1) || (i==10) || (i == 20) || (i == 30)))
			{
				PORT(BUZ_PORT) ^= BUZ_PIN;						// sygna³ dŸwiêkowy x2
			}

			if(!STAND_KEY_DOWN)	// zmiana stanu
			{
				goto check_again;
			}
			_delay_ms(15);
		}
	}

	else if (!(STAND_KEY_DOWN) && (check == 1))	// czujnik w³¹czony
	{
		check = 0;
		lcd_cls();
		lcd_str("stand sensor ON");
		use_buz = 0;

		for(uint8_t i=0; i<200; i++)
		{
			if((buz == 1) && ((i == 1) || (i == 25)))
			{
				PORT(BUZ_PORT) ^= BUZ_PIN;						// sygna³ dŸwiêkowy x1
			}

			if(STAND_KEY_DOWN)
			{
				goto check_again;
			}
			_delay_ms(15);
		}
	}

	lcd_cls();
}
//******************************************************************************************















// *****************************************************************************************
// *                                     KIERUNKOWSKAZY                                    *
// *****************************************************************************************

uint8_t SETblinkers = 0;	// zmienna zapamiêtuj¹ca ustawienie kierunkowskazów

uint8_t i = 0;	// zmienna pozwalaj¹ca zmniejszyæ czêstotliwoœæ licznika

void blinkerskey_init(void)
{
	DDR(BLINKERS_KEY_PORT) &= ~BLINKERS_KEY_PIN;	// ustawienie pinu przycisku kierunkowskazów (WEJŒCIE)
	PORT(BLINKERS_KEY_PORT) |= BLINKERS_KEY_PIN;	// podci¹gniêcie do VCC
}


void blinker_init(void)
{
	DDR(BLINKERS_PORT) |= BLINKERS_PIN;		// ustawienie pinu sygna³owego kierunkowskazów (WYJŒCIE)
	PORT(BLINKERS_PORT) &= ~BLINKERS_PIN;	// ustawienie 0 na pinie
	TIMSK |= (1<<TOIE0);					// zezwolenie na przerwania TIMER0
}


void blinkers_check(void)
{
	if((BLINKERS_KEY_DOWN) && !(SETblinkers == 1))
	{
		SETblinkers = 1;									// <-- w³¹cz kierunkowskazy
		use_buz = 1;
	}

	else if (!(BLINKERS_KEY_DOWN) && (SETblinkers == 1))
	{
		SETblinkers = 0;									// <-- wy³¹cz kierunkowskazy
		use_buz = 1;
	}
}


void blinkers(void)
{
	if(SETblinkers == 1)
	{
		// obs³uga timera
		TCCR0 |= (1<<CS02) | (1<<CS00);			// w³¹cz licznik -> preskaler = 1024
	}

	else if (SETblinkers == 0)
	{
		TCCR0 &= ~(1<<CS02);					// wy³¹cz licznik
		TCCR0 &= ~(1<<CS00);					// ...

		PORT(BLINKERS_PORT) &= ~BLINKERS_PIN;	// wy³¹cz kierunkowskazy
	}
}

// procedura obs³ugi przerwania dla TIMER0
ISR(TIMER0_OVF_vect)
{
	if(i > 14)
	{
		PORT(BLINKERS_PORT) ^= BLINKERS_PIN;	// miganie kierunkowskazami

		i = 0;
	}

	i++;
}
// ****************************************************************************************















// ****************************************************************************************
// *                                         BUZZER                                       *
// ****************************************************************************************

void buzzer_init(void)
{
	DDR(BUZ_PORT) |= BUZ_PIN;		// ustawienie pinu sygna³owego dla buzzera (WYJŒCIE)
	PORT(BUZ_PORT) &= ~BUZ_PIN;		// ustawienie 0 na pinie
}


void buzzer_check(void)
{
	if((buz == 1) && !(use_buz == 0))
	{
		PORT(BUZ_PORT) |= BUZ_PIN;
	}
}


void buzzer_off(void)
{
		PORT(BUZ_PORT) &= ~BUZ_PIN;
		use_buz = 0;
}
// *****************************************************************************************















//							   MANIPULOWANIE WYŒWIETLACZEM
//								|	|	|	|	|	|	|
//								|	|	|	|	|	|	|
//								V	V	V	V	V	V	V

//ZMIENNE GLOBALNE:

uint8_t change = 0;		// zmienna zapamiêtuj¹ca czy treœæ do wyœwietlenia zosta³a zmieniona
						// aby odczekaæ chwilê przy wyœwietlaniu nowej wartoœci

uint8_t content = 0;	// zmienna reprezentuj¹ca treœæ wyœwietlan¹ na ekranie







// ******************************************************************************************
// *                                WYŒWIETLACZ - USTAWIENIA                                *
// ******************************************************************************************

void setkey_init(void)	// inicjalizacja przycisku ustawieñ
{
	DDR(LCD_SET_KEY_PORT) &= ~LCD_SET_KEY_PIN;	// ustawienie pinu przycisku ustawieñ (WEJŒCIE)
	PORT(LCD_SET_KEY_PORT) |= LCD_SET_KEY_PIN;	// podci¹gniêcie do VCC
}


void setkey_check(void)	// sprawdzanie czy przycisk wciœniêty
{
	if(LCD_SET_KEY_DOWN)
	{
		_delay_ms(10);
		if(LCD_SET_KEY_DOWN)
		{
			if(content == 2)
			{
				ref++;			// wybór opcji reflektorów

				if(ref > 1)
				{
					ref = 0;
				}
			}

			if(content == 3)
			{
				buz++;			// wybór opcji buzzera

				if(buz > 1)
				{
					buz = 0;
				}
			}

			use_buz = 1;
			change = 1;	// "zasz³a zmiana"
		}
	}
}
// *****************************************************************************************















// *****************************************************************************************
// *                                  WYŒWIETLACZ - TREŒÆ                                  *
// *****************************************************************************************

uint8_t remember3 = 0;		// zmienna zapamiêtuj¹ca wartoœæ dla grafu



void dispkey_init(void)
{
	DDR(LCD_DISP_KEY_PORT) &= ~LCD_DISP_KEY_PIN;	// ustawienie pinu przycisku wyœwietlacza (WEJŒCIE)
	PORT(LCD_DISP_KEY_PORT) |= LCD_DISP_KEY_PIN;	// podci¹gniêcie do VCC
}


void dispkey_check(void)	// sprawdzanie czy przycisk zosta³ wciœniêty
{
	if(LCD_DISP_KEY_DOWN)
	{
		_delay_ms(10);
		if(LCD_DISP_KEY_DOWN)
		{
			content++;		// zmiana treœci


			if(content > 3)
			{
				content = 0;
			}

			change = 1;		// "zasz³a zmiana"
			use_buz = 1;
		}

	}
}


void display(void)	// wyœwietlanie
{
	if(content == 0)						// prêdkoœæ obr. silnika i temperatura
	{
		lcd_locate(0,0);
		lcd_str("RPM:");
		lcd_locate(0,4);
		lcd_int(rpm);
		lcd_locate(0,9);
		lcd_str("TMP:");
		lcd_locate(0,13);
		lcd_int(tmp);
		lcd_locate(0,15);
		lcd_str("C");

		graph_value = rpm / 500;			// podzia³ka na 16 bloczków (prostok¹tów)


		if(graph_value >= 0 || graph_value <= 16)
		{
			if(!(remember3 == graph_value))			// jeœli wygl¹d grafu nie zmieni³ siê
			{										// .. nie nadpisuj go ponownie

				while(graph_value)					// wyœwietlanie grafu
				{
					lcd_locate(1,graph_value-1);
					lcd_char(255);
					graph_value--;
					_delay_ms(0.1);
				}

				remember3 = graph_value;
			}
		}
	}



	if(content == 1)						// data i godzina
	{
		lcd_locate(0,0);

		switch(day_name)
		{
			case 1:
				lcd_str("Monday");
				break;
			case 2:
				lcd_str("Tuesday");
				break;
			case 3:
				lcd_str("Wednesday");
				break;
			case 4:
				lcd_str("Thursday");
				break;
			case 5:
				lcd_str("Friday");
				break;
			case 6:
				lcd_str("Saturday");
				break;
			case 7:
				lcd_str("Sunday");
				break;
		}

		lcd_locate(0,11);

		if(hour < 10)
		{
			lcd_int(0);
			lcd_locate(0,12);
			lcd_int(hour);
		}

		else
		{
			lcd_int(hour);
		}

		lcd_locate(0,13);
		lcd_char(dot);

		lcd_locate(0,14);

		if(min < 10)
		{
			lcd_int(0);
			lcd_locate(0,15);
			lcd_int(min);
		}

		else
		{
			lcd_int(min);
		}


		lcd_locate(1,0);

		if(day < 10)
		{
			lcd_int(0);
			lcd_locate(1,1);
			lcd_int(day);
		}

		else
		{
			lcd_int(day);
		}

		lcd_locate(1,2);
		lcd_str(".");


		lcd_locate(1,3);

		if(month < 10)
		{
			lcd_int(0);
			lcd_locate(1,4);
			lcd_int(month);
		}

		else
		{
			lcd_int(month);
		}

		lcd_locate(1,5);
		lcd_str(".");

		lcd_locate(1,6);
		lcd_int(year);
	}



	if(content == 2)						// tryb pracy reflektorów
	{
		lcd_locate(0,0);
		lcd_str("headlights mode:");

		if(ref == 0)
		{
			lcd_locate(1,0);
			lcd_str("      AUTO      ");
		}

		if(ref == 1)
		{
			lcd_locate(1,0);
			lcd_str("     MANUAL     ");
		}
	}



	if(content == 3)						// buzzer - w³¹czony / wy³¹czony
	{
		if(buz == 1)
		{
			lcd_locate(0,0);
			lcd_str("speaker: ON     ");
		}

		if(buz == 0)
		{
			lcd_locate(0,0);
			lcd_str("speaker: OFF    ");
		}
	}





	if (change == 1)		// jeœli zasz³a zmiana odczekaj sekundê
	{
		_delay_ms(12);
		PORT(BUZ_PORT) &= ~BUZ_PIN;
		use_buz = 0;
		_delay_ms(488);
		change = 0;
	}
}
// *****************************************************************************************










