/*
 * display.c
 *
 *  Created on: Sep 1, 2025
 *      Author: mmazur
 */

#include "main.h"

// system status variable
extern volatile sys_statuses system_status;

// current menu to display
extern menu_t current_menu;
extern menuoption_t menu_option;

// clock fields to be set
extern clkset_t clock_field;

// auto lights variable (flag)
extern autolights_flag auto_lights;

// stand sensor variable (flag)
extern standsensor_flag stand_sensor;

// delay function target
myDelay_t display_welcome_delay;
myDelay_t display_refresh_delay;
myDelay_t clock_set_field_blinking_delay;

// display refresh flag
bool display_refresh_flag = false;

// clock set field blinking flag
bool clock_set_field_blinking_flag = false;
bool show_field = true;

// buffer for characters to display RPM value as string
char rpm_str[5];

// RTC
char timeStr[8];				// time buffer
char dateStr[16];				// data buffer
char dayStr[4];					// day name buffer
extern RTC_TimeTypeDef gTime;			// get time
extern RTC_DateTypeDef gDate;			// get date
extern const char* weekdays[];


void display_clear() {
	ssd1306_Fill(White);	// white background
	ssd1306_UpdateScreen();
}

void display_refresh() {

	if(display_refresh_flag == false) {
		myDelay_ms_set(&display_refresh_delay,DISPLAY_REFRESH_RATE);
		display_refresh_flag = true;
	}

	if(myDelay_ms_run(&display_refresh_delay)) {
		ssd1306_Fill(White);
		display_refresh_flag = false;
	}
}


// welcome screen
void display_welcome() {

	//display_current_timer_tick = __HAL_TIM_GET_COUNTER(&htim2);
	ssd1306_SetCursor(8,10);
	ssd1306_WriteString("WELCOME",Font_16x24,Black);
	ssd1306_SetCursor(57,40);
	ssd1306_WriteString("PCrs v3.0",Font_7x10,Black);
	ssd1306_UpdateScreen();
}

void display_home_menu() {
	// DATE & TIME
	clock_get();
    // convert to strings
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d",gTime.Hours, gTime.Minutes);
    snprintf(dateStr, sizeof(dateStr), "%02d-%02d-20%02d",gDate.Date, gDate.Month, gDate.Year);
    snprintf(dayStr, sizeof(dayStr), "%s", weekdays[gDate.WeekDay]);

	ssd1306_SetCursor(1,1);
	ssd1306_WriteString(dateStr,Font_7x10,Black);
	ssd1306_SetCursor(1,10);
	ssd1306_WriteString(timeStr,Font_7x10,Black);
	ssd1306_SetCursor(46,10);
	ssd1306_WriteString(dayStr,Font_7x10,Black);
	ssd1306_Line(0,20,74,20,Black);
	ssd1306_Line(0,21,74,21,Black);

	// ENGINE PARAMETERS - RPM
    float rpm = task_RPM_Get();							// get the RPM value
    snprintf(rpm_str, sizeof(rpm_str), "%.0f", rpm);	// convert to string
	ssd1306_SetCursor(64,22);
	ssd1306_WriteString(":",Font_11x18,Black);
	ssd1306_SetCursor(2,22);
	ssd1306_WriteString("ENGINE",Font_11x18,Black);
	ssd1306_SetCursor(6,41);
	ssd1306_WriteString("RPM:",Font_7x10,Black);
	ssd1306_SetCursor(33,41);
	ssd1306_WriteString(rpm_str,Font_7x10,Black);		// display RPM as string

	// ENGINE PARAMETERS - TMP
	ssd1306_SetCursor(78,41);
	ssd1306_WriteString("TMP:",Font_7x10,Black);
	ssd1306_SetCursor(102,41);
	ssd1306_WriteString("90C:",Font_7x10,Black);	//TODO

	// ENGINE PARAMETERS - RPM BAR
	uint16_t bar_length = (task_RPM_Get() / 80);	//	max RPM = 9600; 9600 / 120 = 80		//TODO
	ssd1306_DrawRectangle(6,50,122,62,Black);
	ssd1306_FillRectangle(8,52,10+bar_length,60,Black);

	// ICON BOX
	ssd1306_DrawRectangle(74,0,128,34,Black);
	ssd1306_DrawRectangle(75,0,127,33,Black);

	// ICONS
	if(auto_lights == AL_ON) {
		menu_home_lightbulb();
		menu_home_lightbulb_A();
	}
	else {
		menu_home_lightbulb();
	}

	if(stand_sensor == SS_ON) {
		menu_home_sidestand();
	}
	else {
		menu_home_sidestand();
		menu_home_sidestand_cross();
	}

	menu_home_hazardlights_triangle();	// TODO
}



void display_clock_set_animation() {
	if(menu_option != CLOCK_SET) return;	// don't blink if clock is not setting up

		if(clock_set_field_blinking_flag == false) {
			myDelay_ms_set(&clock_set_field_blinking_delay,500);	// blinking frequency -> 2Hz
			clock_set_field_blinking_flag = true;

			// one time blink, one not
			switch(show_field) {
				case true:	show_field = false;	break;
				case false: show_field = true;	break;
			}
		}

		// time not expired -> draw white rectangles
		if(!myDelay_ms_run(&clock_set_field_blinking_delay)) {
			if(show_field == false) {
				switch(clock_field) {
					case CLKSET_DAY: 		ssd1306_FillRectangle(10,24,32,40,White); 	break;
					case CLKSET_MONTH: 		ssd1306_FillRectangle(40,24,65,40,White);	break;
					case CLKSET_YEAR: 		ssd1306_FillRectangle(75,24,128,40,White);	break;
					case CLKSET_HOUR:   	ssd1306_FillRectangle(10,44,36,60,White);   break;
					case CLKSET_MIN:    	ssd1306_FillRectangle(45,44,70,60,White);  	break;
					case CLKSET_DAYNAME:    ssd1306_FillRectangle(75,44,128,60,White);  break;
				}
			}
		}

		// time not expired -> reset flag
		if(myDelay_ms_run(&clock_set_field_blinking_delay)) {
			clock_set_field_blinking_flag = false;
		}
}


void display_clock_menu() {
	// title
	if(menu_option == CLOCK_SET) {
		ssd1306_SetCursor(14,1);
		ssd1306_WriteString("CLOCK SET",Font_11x18,Black);
	}
	else {
		ssd1306_SetCursor(4,1);
		ssd1306_WriteString("DATE & TIME",Font_11x18,Black);
	}
	ssd1306_Line(0,17,128,17,Black);
	ssd1306_Line(0,18,128,18,Black);
	clock_get();
    // convert to strings
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d",gTime.Hours, gTime.Minutes);
    snprintf(dateStr, sizeof(dateStr), "%02d-%02d-20%02d",gDate.Date, gDate.Month, gDate.Year);
    snprintf(dayStr, sizeof(dayStr), "%s", weekdays[gDate.WeekDay]);

	ssd1306_SetCursor(10,24);
	ssd1306_WriteString(dateStr,Font_11x18,Black);
	ssd1306_SetCursor(12,44);
	ssd1306_WriteString(timeStr,Font_11x18,Black);
	ssd1306_SetCursor(78,44);
	ssd1306_WriteString(dayStr,Font_11x18,Black);

	display_clock_set_animation();
}


void display_lights_menu() {
	// title
	ssd1306_SetCursor(4,1);
	ssd1306_WriteString("AUTO LIGHTS",Font_11x18,Black);
	ssd1306_Line(0,17,128,17,Black);
	ssd1306_Line(0,18,128,18,Black);

	menu_lights_lightbulb();
	ssd1306_SetCursor(25,28);
	if(auto_lights == AL_ON) {
		ssd1306_WriteString("ON",Font_11x18,Black);
		ssd1306_SetCursor(78,28);
		ssd1306_WriteString("A",Font_6x8,Black);
	}
	else {
		ssd1306_WriteString("OFF",Font_11x18,Black);
	}
}


void display_standsensor_menu() {
	// title
	ssd1306_SetCursor(1,1);
	ssd1306_WriteString("STAND",Font_11x18,Black);
	ssd1306_SetCursor(61,1);
	ssd1306_WriteString("SENSOR",Font_11x18,Black);
	ssd1306_Line(0,17,128,17,Black);
	ssd1306_Line(0,18,128,18,Black);

	menu_stand_sidestand();
	ssd1306_SetCursor(25,28);
	if(stand_sensor == SS_ON) {
		ssd1306_WriteString("ON",Font_11x18,Black);
	}
	else {
		ssd1306_WriteString("OFF",Font_11x18,Black);
		menu_stand_sidestand_cross();
	}
}

void display_info_menu() {
	// title
	ssd1306_SetCursor(41,1);
	ssd1306_WriteString("INFO",Font_11x18,Black);
	ssd1306_Line(0,17,128,17,Black);
	ssd1306_Line(0,18,128,18,Black);

	ssd1306_SetCursor(1,20);
	ssd1306_WriteString("Software ver.: 1.0.0",Font_6x8,Black);
	ssd1306_SetCursor(1,29);
	ssd1306_WriteString("Fully open-source",Font_6x8,Black);
	ssd1306_SetCursor(1,38);
	ssd1306_WriteString("by MVZUR",Font_6x8,Black);
	ssd1306_SetCursor(1,47);
	ssd1306_WriteString("https://github.com/",Font_6x8,Black);
	ssd1306_SetCursor(1,56);
	ssd1306_WriteString("MVZUR/PCrs",Font_6x8,Black);


}


void display_menu() {
	menu_update();

	switch (current_menu) {
		case MENU_HOME:
			display_home_menu();
			break;

		case MENU_CLOCK:
			display_clock_menu();
			break;

		case MENU_LIGHTS:
			display_lights_menu();
			break;

		case MENU_STAND:
			display_standsensor_menu();
			break;

		case MENU_INFO:
			display_info_menu();
			break;
	}
}

void display_content() {
	display_refresh();
	display_menu();
	ssd1306_UpdateScreen();
}


void displayer() {
	// wait until SPI clock wakes up
	if(system_status == STATUS_POWER_ON) {
		display_clear();
		// display welcome for 2s - delay time:
		myDelay_ms_set(&display_welcome_delay,2000);
		system_status = STATUS_START;
	}
	else if(system_status == STATUS_START) {
		if(!myDelay_ms_run(&display_welcome_delay)) display_welcome();
		else {
			display_clear();
			system_status = STATUS_WORKING;
		}
	}
	else if(system_status == STATUS_WORKING) {
		display_content();
	}

	// does not work if interrupt occur just before tasker, after displayer (STATUS_TO_SLEEP skipped)
	// TODO to be replaced by hardware power down OLED supply
/*	if(system_status == STATUS_TO_SLEEP) {
		ssd1306_Fill(Black);	// to simulate off
		ssd1306_UpdateScreen();
	}*/
}
