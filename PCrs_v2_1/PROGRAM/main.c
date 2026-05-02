/*
 * main.c
 *
 *  Created on: 17.04.2024
 *      Author: Maksymilian
 */

// header files
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "LCD/lcd_lib.h"
#include "FUN/fun_lib.h"


// function declarations
void clock_init(void);
void idle_init(void);
void sensor_init(void);
void rpm_init(void);
void foto_sig_init(void);
void stand_init(void);
void dispkey_init(void);
void setkey_init(void);
void buzzer_init(void);
void blinker_init(void);
void blinkerskey_init(void);
void lcd_init(void);

void idle(void);
void stand_check(void);
void foto_check(void);
void dispkey_check(void);
void setkey_check(void);
void tmp_check(void);
void blinkers_check(void);
void blinkers(void);
void buzzer_check(void);
void buzzer_off(void);
void display(void);


int main(void)
{


//***********************
//* sensor & system init
//***********************

	sei();				// global interrupt permit
	clock_init();		// <-- CLOCK
	idle_init();		// <-- idle state



	// ****** sensors *******
		sensor_init();			// <-- temperature & light sensors
		rpm_init();				// <-- RPM indicator (spark detection)
		foto_sig_init();		// <-- lights controll
	// **********************


	// ****** systems *******
		stand_init();			// <-- stand sensor key
		dispkey_init();			// <-- display key
		setkey_init();			// <-- display SET key
		buzzer_init();			// <-- piezo
		blinker_init();			// <-- timer for blinkers
		blinkerskey_init();		// <-- blinker key
	// **********************


	while(1)
	{
		idle();		// <-- idle state


	// ***** display welcomes you *****
		_delay_ms(10);
		lcd_init();
		lcd_cls();
		lcd_str("    Welcome!");
		lcd_locate(1,7);
		lcd_str("PCrs v2.2");
		_delay_ms(3000);
		lcd_cls();
	// *********************************


	//*******************************************************************************************
	//* 										PROGRAM 										*
	//*******************************************************************************************
		while(1)
		{
			if(IDLE_ACTIVE) break;

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
