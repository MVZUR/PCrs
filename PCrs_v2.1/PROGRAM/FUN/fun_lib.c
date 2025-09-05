/*
 * fun_lib.c
 *
 *  Created on: 17.04.2024
 *      Author: Maksymilian
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#include "fun_lib.h"
#include "../LCD/lcd_lib.h"

// function declarations
void idle_init(void);
void idle(void);
void clock_init(void);
void rpm_init(void);
void sensor_init(void);
void tmp_check(void);
void foto_sig_init(void);
void foto_check(void);
void stand_init(void);
void stand_check(void);
void blinkerskey_init(void);
void blinker_init(void);
void blinkers_check(void);
void blinkers(void);
void buzzer_init(void);
void buzzer_check(void);
void buzzer_off(void);
void setkey_init(void);
void setkey_check(void);
void dispkey_init(void);
void dispkey_check(void);
void display(void);


//-----------------------
// 		SETTINGS		|
// 	default settings	|
//-------------------------------------------------------------------------------------
uint8_t use_buz = 0;	// buzzer on/off (x - number of signals.)
						// x - buzzer on	0 - buzzer off

uint8_t ref = 0;		// lights mode
						// 1 - MANUAL			0 - AUTO

uint8_t ref_set = 0;	// lights default set in AUTO mode
						// 1 - PASS			0 - LEDS (daily)

uint8_t buz = 1;		// buzzer set
						// 1 - on			0 - off
//-------------------------------------------------------------------------------------





// ******************************************************************************************
// *                                   		IDLE STATE		                                *
// ******************************************************************************************

void idle_init(void)
{
	DDR(IDLE_PORT) &= ~IDLE_PIN;	// idle pin as input
	PORT(IDLE_PORT) |= IDLE_PIN;	// pullup
}


void idle(void)
{
	if(IDLE_ACTIVE)
	{
		lcd_cls();				// clear screen


		TCCR2 &= ~(1<<CS22);	// turn off RPM counter
		TCCR2 &= ~(1<<CS21);	// ...
		TCCR2 &= ~(1<<CS20);	// ...

		GICR &= ~(1<<INT0);		// lock the interrupts on INT0 pin

		ADCSRA &= ~(1<<ADEN);	// turn off ADC



	}

	while(IDLE_ACTIVE)		// IDLE STATE
	{
		blinkers_check();
		blinkers();
	}

	// if not..


	rpm_init();
	ADCSRA |= (1<<ADEN);	// turn ADC on
}
// ******************************************************************************************





// *****************************************************************************************
// *                                       RPM counter                                     *
// *****************************************************************************************

uint8_t spark_sig = 1;					// spark signal
uint8_t check_spark_sig = 0;			// check if spark signal appears
uint32_t delay_between_sparks = 0;		// time between sparks
uint32_t rpm = 0;						// RPM value
uint8_t graph_value = 0;				// to convert rpm RPM value to graphical representation (as 16 rectangles)
uint8_t frequency_cutter2 = 0;			// variable that reduce rectangles refresh rate to prevent 'ghosting'



void rpm_init(void)
{
	DDR(RPM_PORT) &= ~RPM_PIN;		// RPM pin as input (INT0 -> PD2)

	TCCR2 |= (1<<CS22);	// turn on RPM counter
	TCCR2 |= (1<<CS21);	// ...
	TCCR2 |= (1<<CS20);	// ...

	TIMSK |= (1<<TOIE2);			// allow interrupts - TIMER2

	GICR |= (1<<INT0);				// unlock INT0 interrupts

	MCUCR |= (1<<ISC01);			// interrupt on falling edge
	MCUCR &= ~(1<<ISC00);			// ...
}


// INT0 interrupt handler
ISR(INT0_vect)
{
	spark_sig++;

	if(frequency_cutter2 > 10)
	{
		frequency_cutter2 = 0;
	}

	TCCR2 |= (1<<CS20);

	if(spark_sig > 50)			// time measured between 50 sparks
	{
		spark_sig = 0;


		TCCR2 &= ~(1<<CS22);	// turn off buzzer counter
		TCCR2 &= ~(1<<CS21);	// ...
		TCCR2 &= ~(1<<CS20);	// ...

	}

	if(delay_between_sparks == 0)
	{
		_delay_ms(1);			// delay necessary to make measurement correctly
	}

	else
	{
		if((3570000/delay_between_sparks) < 8000)
		{
			if(frequency_cutter2 == 0)
			{
				rpm = 3570000/delay_between_sparks;		// time conversion
			}											// .. to RPM value

		}
		delay_between_sparks = 0;
	}

	frequency_cutter2++;
}


// TIMER2 interrupt handler
ISR(TIMER2_OVF_vect)
{
	delay_between_sparks++;						// <-- measure of time between sparks
}
// *****************************************************************************************





// *****************************************************************************************
// *                                         CLOCK                                         *
// *****************************************************************************************

// -------------------
// SETTING THE CLOCK |
// -------------------
uint8_t sec = 0;				// <-- secs
uint8_t min = 58;				// <-- mins
uint8_t hour = 18;				// <-- hours
uint8_t day = 28;				// <-- days
uint8_t day_name = 5;			// <-- 1-monday / 2-tuesday / 3-wednesday / 4-thursday / 5-friday / 6-saturday / 7-sunday
uint8_t month = 7;				// <-- months
uint8_t feb_correction = 3;		// <-- leap year in february
uint16_t year = 2023;			// <-- years
// --------------------

// operational variables
char dot = 58;					// blinking dots

// setting the clock manually
uint8_t mark1 = 0;
uint8_t mark2 = 0;
uint8_t clock_set = 0;


void clock_init(void)
{
	// TIMER1 init
	TCCR1B |= (1<<WGM12);	// mode CTC
	TCCR1B |= (1<<CS12);	// prescaler 256
	OCR1A = 31249;			// copare register
	TIMSK |= (1<<OCIE1A);	// interrupt enable
}


// TIMER1 interrupt handler -> CLOCK
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

	if(month == 2)						// <-- february
	{
		if(feb_correction == 4)			// <<< leap year
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



	// to RPM indication:
	// check if the spark signal is present

	if(spark_sig == check_spark_sig)	// if spark signal do not change for 1 sec
	{									// ...
		rpm = 0;						// clear rpm value (engine stopped)
	}

	spark_sig = check_spark_sig;



	// ---------- SETTING THE CLOCK MANUALLY -----------------------------------------------

	if(LCD_SET_KEY_DOWN)
	{
		mark1++;		// count to 3
	}
	else
	{
		mark1 = 0;		// if SET key is released, clear
	}

	if(mark1 == 3)		// if not, run setting the clock procedure
	{
		PORT(BUZ_PORT) &= ~BUZ_PIN;

		// make Timer1 faster
		TCCR1B &= ~(1<<CS12);	// prescaler 64
		TCCR1B |= (1<<CS11);	// ...
		TCCR1B |= (1<<CS10);	// ...


		clock_set = 1;			// SETTING THE CLOCK PROCEDURE

		while(clock_set == 1)
		{

			lcd_locate(0,13);	// if dots between hours & mins disappear,
			lcd_str(":");		// show them again

			_delay_ms(100);		// calmly..

			// NAME OF THE DAY ***********************
			lcd_locate(0,0);
			lcd_str("           ");
			_delay_ms(1000);
			lcd_locate(0,0);
			switch(day_name)	// DISPLAY
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

			while(1) // <-- day name adjustment
			{
				if(LCD_DISP_KEY_DOWN)	// SETkey - increase day number (name) by 1
				{
					day_name++;


					if(day_name > 7)
					{
						day_name = 1;
					}

					PORT(BUZ_PORT) |= BUZ_PIN;	// buzzer on

					lcd_locate(0,0);
					lcd_str("           ");		// <-- to show what is gonna to be change (blinking)
					lcd_locate(0,0);
					switch(day_name)	// display new value
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
					PORT(BUZ_PORT) &= ~BUZ_PIN;	// buzzer off
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




			// HOURS ***********************
			lcd_locate(0,11);
			lcd_str("  ");
			_delay_ms(1000);

			lcd_locate(0,11);
			if(hour < 10)		// DISPLAY
			{
				lcd_int(0);
				lcd_locate(0,12);
				lcd_int(hour);
			}
			else
			{
				lcd_int(hour);
			}


			while(1) // <-- hour adjustment
			{
				if(LCD_DISP_KEY_DOWN) // SETkey - increase hour by 1
				{
					hour++;

					if(hour > 23)
					{
						hour = 0;
					}

					PORT(BUZ_PORT) |= BUZ_PIN;


					lcd_locate(0,11);

					if(hour < 10)		// display new value
					{
						lcd_int(0);
						lcd_locate(0,12);
						lcd_int(hour);
					}
					else
					{
						lcd_int(hour);	// (without 0)
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




			// MINUTES ***********************
			lcd_locate(0,14);
			lcd_str("  ");
			_delay_ms(1000);

			lcd_locate(0,14);
			if(min < 10)		// DISPLAY (with 0 before number)
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
				if(LCD_DISP_KEY_DOWN)	// <- minute adjustment
				{
					min++;

					if(min > 59)
					{
						min = 0;
					}

					PORT(BUZ_PORT) |= BUZ_PIN;


					lcd_locate(0,14);

					if(min < 10)		// display new value (with 0 before number)
					{
						lcd_int(0);
						lcd_locate(0,15);
						lcd_int(min);
					}
					else
					{

						lcd_int(min);	// (without 0)
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



			// DAYS ***********************
			lcd_locate(1,0);
			lcd_str("  ");
			_delay_ms(1000);


			lcd_locate(1,0);
			if(day < 10)		// DISPLAY
			{
				lcd_int(0);
				lcd_locate(1,1);
				lcd_int(day);
			}

			else
			{
				lcd_int(day);
			}

			while(1)	// <- day adjustment
			{
				if(LCD_DISP_KEY_DOWN)
				{
					day++;

					if(month == 2)						// <-- february
					{
						if(feb_correction == 4)			// <<< leap year
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
					if(day < 10)		// display new value
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



			// MONTHS ***********************
			lcd_locate(1,3);
			lcd_str("  ");
			_delay_ms(1000);


			lcd_locate(1,3);
			if(month < 10)			// DISPLAY
			{
				lcd_int(0);
				lcd_locate(1,4);
				lcd_int(month);
			}

			else
			{
				lcd_int(month);
			}

			while(1)	// <- months adjustment
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
					if(month < 10)			// display new value
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



			// YEARS ***********************
			lcd_locate(1,6);
			lcd_str("    ");
			_delay_ms(1000);


			lcd_locate(1,6);	// DISPLAY
			lcd_int(year);

			while(1)	// <- years adjustment
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


					lcd_locate(1,6);	// display new value
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

		TCCR1B |= (1<<CS12);	// prescaler 256
		TCCR1B &= ~(1<<CS11);	// ...
		TCCR1B &= ~(1<<CS10);	// ...

		mark1 = 0;

	}
}
// *****************************************************************************************





// *****************************************************************************************
// *                                         SENSORS                                       *
// *****************************************************************************************

uint8_t tmp = 0;
uint16_t tmp_value = 0;
uint16_t foto_value = 0;

uint8_t remember1 = 99;		// variables to save previous state of ref
uint8_t remember2 = 99;		// to prevent repeats

uint8_t frequency_cutter = 0;



void sensor_init(void)
{
	// ADC init
	ADMUX |= (1<<REFS1) | (1<<REFS0);			// initial ref voltage source (2,56V)
	ADCSRA |= (1<<ADEN);						// ADC on
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1);			// prescaller 64
}


void tmp_check(void)
{
	// channel ADC0
	ADMUX = (ADMUX & 0xF8) | 0;

	ADCSRA |= (1<<ADSC);			// start of measurements
	while( ADCSRA & (1<<ADSC) );	// wait for end of the measurements

	tmp_value = (ADCW);				// value measured by ADC

	if(frequency_cutter > 200)		// update ADC value with freq/200
	{								// to prevent 'ghosting'
		frequency_cutter = 0;
	}

	if(frequency_cutter == 0)		// ...
	{
		// convert ADC value to temperature
		tmp = (105 - ((0.09)*tmp_value));
	}

	frequency_cutter++;
}


void foto_sig_init(void)
{
	DDR(FOTO_SIG_MODE_PORT) |= FOTO_SIG_MODE_PIN;	// set pin to AUTO-MANUAL lights relay as output
	DDR(FOTO_SIG_LIGHT_PORT) |= FOTO_SIG_LIGHT_PIN;	// set pin to LED-PASS lights relay as output
}


void foto_check(void)
{
	// channel ADC1
	ADMUX = (ADMUX & 0xF8) | 1;

	ADCSRA |= (1<<ADSC);			// start of measurements
	while( ADCSRA & (1<<ADSC) );	// wait for end of the measurements
	foto_value = (ADCW);			// value measured by ADC

	if((ref == 0) && !(ref == remember1))		// AUTO mode + prevent repeats
	{
		PORT(FOTO_SIG_MODE_PORT) |= FOTO_SIG_MODE_PIN;	// relay -> AUTO mode

		remember1 = ref;
	}

	else if((ref == 1) && !(ref == remember1))
	{
		PORT(FOTO_SIG_MODE_PORT) &= ~FOTO_SIG_MODE_PIN;	// relay -> MANUAL mode

		remember1 = ref;
	}

	if(ref == 0)					// lights mode - AUTO
	{
		if((foto_value > 190) && !(remember2 == 1) )
		{
			PORT(FOTO_SIG_LIGHT_PORT) |= FOTO_SIG_LIGHT_PIN;	// pass lights

			remember2 = 1;
		}

		else if((foto_value <= 170) && !(remember2 == 0))
		{
			PORT(FOTO_SIG_LIGHT_PORT) &= ~FOTO_SIG_LIGHT_PIN;	// LED lights

			remember2 = 0;
		}
	}
}
// *****************************************************************************************





// *****************************************************************************************
// *                                     	SIDE STAND                                     *
// *****************************************************************************************

uint8_t check = 0;	// stand state was checked?
					// to prevent display the same information


void stand_init(void)	// stand button init
{
	DDR(STAND_KEY_PORT) &= ~STAND_KEY_PIN;	// set as input
	PORT(STAND_KEY_PORT) |= STAND_KEY_PIN;	// pullup
}


void stand_check(void)	// check state of stand button
{
	check_again:

	if(STAND_KEY_DOWN && !(check == 1))	// sensor OFF
	{
		check = 1;
		lcd_cls();
		lcd_str("stand sensor OFF");
		use_buz = 0;

		for(uint8_t i=0; i<200; i++)
		{
			if((buz == 1) && ((i == 1) || (i==10) || (i == 20) || (i == 30)))
			{
				PORT(BUZ_PORT) ^= BUZ_PIN;						// sound signal
			}

			if(!STAND_KEY_DOWN)	// state was changed during sound signal
			{
				goto check_again;
			}
			_delay_ms(15);
		}
	}

	else if (!(STAND_KEY_DOWN) && (check == 1))	// sensor ON
	{
		check = 0;
		lcd_cls();
		lcd_str("stand sensor ON");
		use_buz = 0;

		for(uint8_t i=0; i<200; i++)
		{
			if((buz == 1) && ((i == 1) || (i == 25)))
			{
				PORT(BUZ_PORT) ^= BUZ_PIN;						// sound signal x1
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
// *                                     	BLINKERS                                       *
// *****************************************************************************************

uint8_t SETblinkers = 0;	// variable saving blinkers state

uint8_t i = 0;	// variable to reduce timer frequency

void blinkerskey_init(void)
{
	DDR(BLINKERS_KEY_PORT) &= ~BLINKERS_KEY_PIN;	// blinkers button pin as input
	PORT(BLINKERS_KEY_PORT) |= BLINKERS_KEY_PIN;	// pullup
}


void blinker_init(void)
{
	DDR(BLINKERS_PORT) |= BLINKERS_PIN;		// blinkers signal pin as output
	PORT(BLINKERS_PORT) &= ~BLINKERS_PIN;	// set lo
	TIMSK |= (1<<TOIE0);					// allow interrupts for TIMER0
}


void blinkers_check(void)
{
	if((BLINKERS_KEY_DOWN) && !(SETblinkers == 1))
	{
		SETblinkers = 1;									// <-- blinkers ON
		use_buz = 1;
	}

	else if (!(BLINKERS_KEY_DOWN) && (SETblinkers == 1))
	{
		SETblinkers = 0;									// <-- blinkers OFF
		use_buz = 1;
	}
}


void blinkers(void)
{
	if(SETblinkers == 1)
	{
		// timer control
		TCCR0 |= (1<<CS02) | (1<<CS00);			// timer ON -> prescaler = 1024
	}

	else if (SETblinkers == 0)
	{
		TCCR0 &= ~(1<<CS02);					// timer OFF
		TCCR0 &= ~(1<<CS00);					// ...

		PORT(BLINKERS_PORT) &= ~BLINKERS_PIN;	// blinkers OFF
	}
}

// TIMER0 interrupt handler
ISR(TIMER0_OVF_vect)
{
	if(i > 14)
	{
		PORT(BLINKERS_PORT) ^= BLINKERS_PIN;	// blinkers blinking

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
	DDR(BUZ_PORT) |= BUZ_PIN;		// buzzer pin as output
	PORT(BUZ_PORT) &= ~BUZ_PIN;		// set lo
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










//							   	   DISPLAY MANIPULATION
//								|	|	|	|	|	|	|
//								|	|	|	|	|	|	|
//								V	V	V	V	V	V	V

//GLOBAL VARIABLES:

uint8_t change = 0;		// variable for display content (is it changed?)
						// to wait some time before displaying new value

uint8_t content = 0;	// variable that represents content currently displaying on screen





// ******************************************************************************************
// *                                	DISPLAY - SETTINGS                                  *
// ******************************************************************************************

void setkey_init(void)	// init set key
{
	DDR(LCD_SET_KEY_PORT) &= ~LCD_SET_KEY_PIN;	// as input
	PORT(LCD_SET_KEY_PORT) |= LCD_SET_KEY_PIN;	// pullup
}


void setkey_check(void)	// is it pressed?
{
	if(LCD_SET_KEY_DOWN)
	{
		_delay_ms(10);
		if(LCD_SET_KEY_DOWN)
		{
			if(content == 2)
			{
				ref++;			// switch light mode

				if(ref > 1)
				{
					ref = 0;
				}
			}

			if(content == 3)
			{
				buz++;			// switch buzzer mode

				if(buz > 1)
				{
					buz = 0;
				}
			}

			use_buz = 1;
			change = 1;	// "there has been a change"
		}
	}
}
// *****************************************************************************************





// *****************************************************************************************
// *                                  DISPLAY - CONTENT                                    *
// *****************************************************************************************

uint8_t remember3 = 0;		// variable that represents graph value



void dispkey_init(void)		// display key init
{
	DDR(LCD_DISP_KEY_PORT) &= ~LCD_DISP_KEY_PIN;	// as input
	PORT(LCD_DISP_KEY_PORT) |= LCD_DISP_KEY_PIN;	// pullup
}


void dispkey_check(void)	// is it pressed?
{
	if(LCD_DISP_KEY_DOWN)
	{
		_delay_ms(10);
		if(LCD_DISP_KEY_DOWN)
		{
			content++;		// switch content


			if(content > 3)
			{
				content = 0;
			}

			change = 1;		// "there has been a change"
			use_buz = 1;
		}

	}
}


void display(void)	// displaying
{
	if(content == 0)						// RPM & engine temperature
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

		graph_value = rpm / 500;			// graph value of RPM (16 rectangles)


		if(graph_value >= 0 || graph_value <= 16)
		{
			if(!(remember3 == graph_value))			// if graph value do not change
			{										// .. do not update it on screen

				while(graph_value)					// graph displaying
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



	if(content == 1)						// date & time
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



	if(content == 2)						// lights mode
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



	if(content == 3)						// buzzer - ON / OFF
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





	if (change == 1)		// if there has been a change, wait a sec
	{
		_delay_ms(12);
		PORT(BUZ_PORT) &= ~BUZ_PIN;
		use_buz = 0;
		_delay_ms(488);
		change = 0;
	}
}
// *****************************************************************************************




