/*
 * pcrs_menu.c
 *
 *  Created on: Sep 14, 2025
 *      Author: mmazur
 */


#include "main.h"


// auto lights variable (flag)
extern autolights_flag auto_lights;

// stand sensor variable (flag)
extern standsensor_flag stand_sensor;

// menu current state variables
menu_t current_menu = MENU_HOME;
menuoption_t menu_option = NONE;

// variable to determine when button were pressed
static uint32_t menu_button_pressed_tick=0;
static uint32_t ok_button_pressed_tick=0;

// CLOCK
// clock fields to be set
clkset_t clock_field = CLKSET_DAY;




// MENU structure
void menu_update()
{
	// MENU button operations
	// determine what to do (go forward or backward in MENU)
	if((READ_BUTTON_MENU == 0) && (menu_button_pressed_tick == 0)) {
		menu_button_pressed_tick = HAL_GetTick();	// SYSTICK used to determine button press time
	}
	else {
		if((READ_BUTTON_MENU == 1) && (menu_button_pressed_tick != 0)) {
			uint32_t current_tick = HAL_GetTick();

			if(current_tick - menu_button_pressed_tick < 80) return;	// contact debouncing (80ms)

			else if(current_tick - menu_button_pressed_tick < MENU_HOME_RETURN_DELAY_TIME) {

				// if not setting the clock, switch menu
				if(menu_option != CLOCK_SET) {
					switch (current_menu) {
						case MENU_HOME:		current_menu = MENU_CLOCK; 	break;
						case MENU_CLOCK: 	current_menu = MENU_LIGHTS; break;
						case MENU_LIGHTS: 	current_menu = MENU_STAND;	break;
						case MENU_STAND: 	current_menu = MENU_INFO;	break;
						case MENU_INFO:     current_menu = MENU_HOME;   break;
					}
				}
				else {
					switch (clock_field) {
						//case CLKSET_NONE:		clock_field = CLKSET_DAY; 		break;
						case CLKSET_DAY: 		clock_field = CLKSET_MONTH; 	break;
						case CLKSET_MONTH: 		clock_field = CLKSET_YEAR;		break;
						case CLKSET_YEAR: 		clock_field = CLKSET_HOUR;		break;
						case CLKSET_HOUR:   	clock_field = CLKSET_MIN;   	break;
						case CLKSET_MIN:    	clock_field = CLKSET_DAYNAME;  	break;
						case CLKSET_DAYNAME:    menu_option = NONE;				break;

					}
				}

				menu_button_pressed_tick = 0;
			}
			else {
				// longer than 2sec -> return home, reset menu option
				current_menu = MENU_HOME;
				menu_option = NONE;
				menu_button_pressed_tick = 0;
			}
		}
	}


    // TODO OK button operations
    if ((READ_BUTTON_OK == 0 && ok_button_pressed_tick == 0)) {
    	ok_button_pressed_tick = HAL_GetTick();	// SYSTICK used to determine button press time
    }
	if((READ_BUTTON_OK == 1) && (ok_button_pressed_tick != 0)) {
		uint32_t current_tick = HAL_GetTick();

		if(current_tick - ok_button_pressed_tick < 80) return;	// contact debouncing (80ms)

		else {
			switch (current_menu) {
				case MENU_HOME:		menu_option = NONE;	break;

				case MENU_CLOCK:
					if(menu_option == NONE) {
						menu_option = CLOCK_SET;
						clock_field = CLKSET_DAY; 	break;
					}
					if(menu_option == CLOCK_SET) {
						// setting the clock
						menu_setting_clock();	break;
					}
				case MENU_LIGHTS:
					// switch between auto lights working mode ON/OFF
					if(auto_lights == AL_ON) {
						auto_lights = AL_OFF; break;
					}
					else {
						auto_lights = AL_ON; break;
					}
				case MENU_STAND:
					// switch between stand sensor working mode ON/OFF
					if(stand_sensor == SS_ON) {
						stand_sensor = SS_OFF; break;
					}
					else {
						stand_sensor = SS_ON; break;
					}
				case MENU_INFO:     menu_option = NONE; break;

			}
			ok_button_pressed_tick = 0;
		}
	}
}
