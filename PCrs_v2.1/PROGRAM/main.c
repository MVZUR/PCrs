/*
 * main.c
 *
 *  Created on: 17.04.2024
 *      Author: Maksymilian
 */

// pliki nag��wkowe
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "LCD/lcd_lib.h"
#include "Functions/fun_lib.h"


int main(void)
{

//****************************************************
//* inicjalizacja system�w / ustawienia pin�w port�w *
//****************************************************

	sei();				// globalne zezwolenie na przerwania

	clock_init();		// <-- ZEGAR

	idle_init();		// <-- pin syg. do uruchomienia stanu bezczynno�ci



	// ****** czujniki ******
		sensor_init();			// <-- czujniki temperatury i o�wietlenia
		rpm_init();				// <-- detekcja iskry ( wska�nik RPM )
		foto_sig_init();		// <-- sterowanie reflektorami
	// **********************


	// systemy
		stand_init();			// <-- przycisk czujnika stopki
		dispkey_init();			// <-- przycisk zmiany tre�ci na ekranie
		setkey_init();			// <-- przycisk zmiany ustawie�
		buzzer_init();			// <-- sterowanie buzzerem (sygna�y d�wi�kowe)
		blinker_init();			// <-- STEROWNIK - oprog. migaj�ce kierunkowskazami
		blinkerskey_init();		// <-- przycisk sterownika kierunkowskaz�w
	// **********************


	while(1)
	{
		idle();				// <-- stan bezczynno�ci

	// ***** wy�wietlacz *****
		_delay_ms(10);
		lcd_init();
		lcd_cls();
		lcd_str("    Welcome!");
		lcd_locate(1,7);
		lcd_str("PCrs v1.0");
		_delay_ms(3000);
		lcd_cls();
	// ***********************




	//*******************************************************************************************
	//* 										PROGRAM 										*
	//*******************************************************************************************
		while(1)
		{
			if(IDLE_ACTIVE)
			{
				break;
			}

			stand_check();
			foto_check();
			dispkey_check();
			setkey_check();
			tmp_check();
			blinkers_check();
			blinkers();
			buzzer_check();
			display();



			buzzer_off();
			for(uint8_t i=12; i>0; i--)
			{
				buzzer_check();
				_delay_ms(1);
			}
			buzzer_off();
		}
	}
}
