/*
 * main.c
 *
 *  Created on: 17.04.2024
 *      Author: Maksymilian
 */

// pliki nag³ówkowe
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "LCD/lcd_lib.h"
#include "Functions/fun_lib.h"


int main(void)
{

//****************************************************
//* inicjalizacja systemów / ustawienia pinów portów *
//****************************************************

	sei();				// globalne zezwolenie na przerwania

	clock_init();		// <-- ZEGAR

	idle_init();		// <-- pin syg. do uruchomienia stanu bezczynnoœci



	// ****** czujniki ******
		sensor_init();			// <-- czujniki temperatury i oœwietlenia
		rpm_init();				// <-- detekcja iskry ( wskaŸnik RPM )
		foto_sig_init();		// <-- sterowanie reflektorami
	// **********************


	// systemy
		stand_init();			// <-- przycisk czujnika stopki
		dispkey_init();			// <-- przycisk zmiany treœci na ekranie
		setkey_init();			// <-- przycisk zmiany ustawieñ
		buzzer_init();			// <-- sterowanie buzzerem (sygna³y dŸwiêkowe)
		blinker_init();			// <-- STEROWNIK - oprog. migaj¹ce kierunkowskazami
		blinkerskey_init();		// <-- przycisk sterownika kierunkowskazów
	// **********************


	while(1)
	{
		idle();				// <-- stan bezczynnoœci

	// ***** wyœwietlacz *****
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
