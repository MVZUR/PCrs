/*
 * fun_lib.h
 *
 *  Created on: 17.04.2024
 *      Author: Maksymilian
 */

#ifndef FUNCTIONS_FUN_LIB_H_
#define FUNCTIONS_FUN_LIB_H_





//****************************
//* definicje pinów i portów *
//**************************************************************************************************

#define IDLE_PORT C									// <-- port sygna³u wprowadzaj¹cego CPU w stan bezczynnoœci
#define IDLE_PIN (1<<PC0)							// <-- pin sygna³u idle
#define IDLE_ACTIVE !(PINC & IDLE_PIN) 				// makrodef sprawdzaj¹ca, czy jest stan niski 0

#define RPM_PORT D									// <-- port pomiaru prêdkoœci obrotowej silnika
#define RPM_PIN (1<<PD2)							// <-- pin pomiaru prêdkoœci obrotowej silnika (INT0)
#define RPM_SIG_DETECTION (PIND & RPM_PIN) !=0		// makrodef sprawdzaj¹ca, czy jest stan wysoki 1

#define TMP_PORT A									// <-- port pomiaru temperatury
#define TMP_PIN (1<<PA0)							// <-- pin pomiaru temperatury (ADC)

#define FOTO_PORT A									// <-- port czujnika zmierzchowego
#define FOTO_PIN (1<<PA1)							// <-- pin czujnika zmierzchowego (ADC)

#define FOTO_SIG_MODE_PORT B						// <-- port sygna³u przekaŸnika ref. AUTO-MANUAL
#define FOTO_SIG_MODE_PIN (1<<PB3)					// <-- pin sygna³u przekaŸnika reflektorów

#define FOTO_SIG_LIGHT_PORT B						// <-- port sygna³u do ¿arówek reflektorów w trybie AUTO
#define FOTO_SIG_LIGHT_PIN (1<<PB4)					// <-- pin sygna³u do ¿arówek LED-mijania

#define BLINKERS_PORT D								// <-- port sygna³u do uruchomienia migania dwoma kierunkowskazami na raz
#define BLINKERS_PIN (1<<PD0)						// <-- pin sygna³u do uruchomienia migania dwoma kierunkowskazami na raz (licznik)

#define BUZ_PORT B									// <-- port sygna³u buzzera
#define BUZ_PIN (1<<PB2)							// <-- pin sygna³u buzzera (licznik)

#define BLINKERS_KEY_PORT D								// <-- port przycisku do uruchomienia migania dwoma kierunkowskazami na raz
#define BLINKERS_KEY_PIN (1<<PD1)						// <-- pin przycisku do uruchomienia migania dwoma kierunkowskazami na raz
#define BLINKERS_KEY_DOWN !(PIND & BLINKERS_KEY_PIN)	// makrodef sprawdzaj¹ca, czy jest stan niski 0

#define LCD_DISP_KEY_PORT D								// <-- port przycisku do zmiany treœci na ekranie
#define LCD_DISP_KEY_PIN (1<<PD3)						// <-- pin przycisku do zmiany treœci na ekranie
#define LCD_DISP_KEY_DOWN !(PIND & LCD_DISP_KEY_PIN) 	// makrodef sprawdzaj¹ca, czy jest stan niski 0

#define LCD_SET_KEY_PORT D								// <-- port przycisku do zmiany ustawieñ
#define LCD_SET_KEY_PIN (1<<PD4)						// <-- pin przycisku do zmiany ustawieñ
#define LCD_SET_KEY_DOWN !(PIND & LCD_SET_KEY_PIN) 		// makrodef sprawdzaj¹ca, czy jest stan niski 0

#define STAND_KEY_PORT B								// <-- port przycisku od³¹czenia czujnika podnó¿ka
#define STAND_KEY_PIN (1<<PB1)							// <-- pin przycisku od³¹czenia czujnika podnó¿ka
#define STAND_KEY_DOWN !(PINB & STAND_KEY_PIN)		 	// makrodef sprawdzaj¹ca, czy jest stan niski 0

//**************************************************************************************************


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


#endif /* FUNCTIONS_FUN_LIB_H_ */
