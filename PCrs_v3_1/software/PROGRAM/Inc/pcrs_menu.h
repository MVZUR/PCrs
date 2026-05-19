/*
 * pcrs_menu.h
 *
 *  Created on: Sep 14, 2025
 *      Author: mmazur
 */

#ifndef INC_PCRS_MENU_H_
#define INC_PCRS_MENU_H_


// button read state defines
#define READ_BUTTON_MENU	HAL_GPIO_ReadPin(GPIO_Input_BUTTONmenu_GPIO_Port, GPIO_Input_BUTTONmenu_Pin)
#define READ_BUTTON_OK		HAL_GPIO_ReadPin(GPIO_Input_BUTTONok_GPIO_Port, GPIO_Input_BUTTONok_Pin)

// menu button pressing time to return home in menu[ms]
#define MENU_HOME_RETURN_DELAY_TIME 1000U


// menu parameters
typedef enum {
    MENU_HOME,       	// time & date, RPM, temperature
    MENU_CLOCK,   		// setting up time & date, switch lights mode, stand sensor on/off
	MENU_LIGHTS,
	MENU_STAND,
    MENU_INFO        	// about firmware
} menu_t;


// options in current menu
typedef enum {
	NONE,
    CLOCK_SET,
/*	AUTO_LIGHTS_ON,
	AUTO_LIGHTS_OFF,*/
	STAND_DETECT_ON,
	STAND_DETECT_OFF
} menuoption_t;



// clock fields to be set
typedef enum {
	//CLKSET_NONE,
	CLKSET_DAY,
    CLKSET_MONTH,
	CLKSET_YEAR,
	CLKSET_HOUR,
	CLKSET_MIN,
	CLKSET_DAYNAME
} clkset_t;





#endif /* INC_PCRS_MENU_H_ */
