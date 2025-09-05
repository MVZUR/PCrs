/*
 * fun_lib.h
 *
 *  Created on: 17.04.2024
 *      Author: Maksymilian
 */

#ifndef FUNCTIONS_FUN_LIB_H_
#define FUNCTIONS_FUN_LIB_H_





//***************************
//* pins & ports definitons *
//**************************************************************************************************

#define IDLE_PORT C									// <-- idle signal port that puts CPU in idle state
#define IDLE_PIN (1<<PC0)							// <-- idle signal pin
#define IDLE_ACTIVE (PINC & IDLE_PIN) 				// if hi, active

#define RPM_PORT D									// <-- RPM (spark) measurement port
#define RPM_PIN (1<<PD2)							// <-- RPM measurement pin (INT0)
#define RPM_SIG_DETECTION (PIND & RPM_PIN) 			// if hi, active

#define TMP_PORT A									// <-- temperature measurement port
#define TMP_PIN (1<<PA0)							// <-- temp measurement pin (ADC)

#define FOTO_PORT A									// <-- photoresistor port
#define FOTO_PIN (1<<PA1)							// <-- photoresistor pin (ADC)

#define FOTO_SIG_MODE_PORT B						// <-- light mode AUTO-MANUAL (signal for relay) port
#define FOTO_SIG_MODE_PIN (1<<PB3)					// <-- signal for relay pin

#define FOTO_SIG_LIGHT_PORT B						// <-- light mode LED-PASS in AUTO mode (signal for relay) port
#define FOTO_SIG_LIGHT_PIN (1<<PB4)					// <-- signal for relay pin

#define BLINKERS_PORT B								// <-- blinkers BLINKING square signal (signal for relay) port
#define BLINKERS_PIN (1<<PB2)						// <-- signal for relay pin (TIMER)

#define BUZ_PORT D									// <-- buzzer signal port
#define BUZ_PIN (1<<PD7)							// <-- buzzer signal pin

#define BLINKERS_KEY_PORT D								// <-- blinkers key port
#define BLINKERS_KEY_PIN (1<<PD5)						// <-- blinkers key pin
#define BLINKERS_KEY_DOWN !(PIND & BLINKERS_KEY_PIN)	// if lo, active

#define LCD_DISP_KEY_PORT D								// <-- display key port (to change display content)
#define LCD_DISP_KEY_PIN (1<<PD3)						// <-- display key pin
#define LCD_DISP_KEY_DOWN !(PIND & LCD_DISP_KEY_PIN) 	// if lo, active

#define LCD_SET_KEY_PORT D								// <-- set key port (to change settings on display)
#define LCD_SET_KEY_PIN (1<<PD4)						// <-- set key pin
#define LCD_SET_KEY_DOWN !(PIND & LCD_SET_KEY_PIN) 		// if lo, active

#define STAND_KEY_PORT D								// <-- stand sensor key port
#define STAND_KEY_PIN (1<<PD6)							// <-- stand sensor key pin
#define STAND_KEY_DOWN !(PIND & STAND_KEY_PIN)		 	// if lo, active

//**************************************************************************************************


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


#endif /* FUNCTIONS_FUN_LIB_H_ */
