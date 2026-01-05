/*
 * pcrs_clock.c
 *
 *  Created on: Sep 24, 2025
 *      Author: mmazur
 */
#include "main.h"


extern RTC_HandleTypeDef hrtc;	// RTC handle

// set time & date
RTC_TimeTypeDef sTime = {0};	// set time
RTC_DateTypeDef sDate = {0};	// set date
RTC_TimeTypeDef gTime = {0};	// get time
RTC_DateTypeDef gDate = {0};	// get date
extern clkset_t clock_field;	// what will be setting

void clock_init() {
	sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
	sDate.Month = RTC_MONTH_JANUARY;
	sDate.Date = 1;
	sDate.Year = 25;
	sTime.Hours = 0;
	sTime.Minutes = 0;
	sTime.Seconds = 0;

	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}




const char* weekdays[] = {
    "???",      // 0 index – unused
    "Mon",   	// 1
    "Tue",  	// 2
    "Wed",		// 3
    "Thu", 		// 4
    "Fri",   	// 5
    "Sat", 		// 6
    "Sun"    	// 7
};

uint8_t days_in_month[] = {
	0,	// index 0 - unused
    31, // Jan
    28, // Feb (29 in leap year)
    31, // Mar
    30, // Apr
    31, // May
    30, // Jun
    31, // Jul
    31, // Aug
    30, // Sep
    31, // Oct
    30, // Nov
    31  // Dec
};

void clock_get() {
	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
}

void clock_set() {
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}



bool is_leap_year(uint8_t year) {
    uint16_t full_year = 2000 + year;  // RTC gives "25" = 2025
    return ((full_year % 4 == 0 && full_year % 100 != 0) || (full_year % 400 == 0));
}



void menu_setting_clock() {
	if(clock_field == CLKSET_DAY) {
		sDate.Date++;
		if(sDate.Date > days_in_month[gDate.Month] ) {
			sDate.Date = 1;
		}
		clock_set();
	}
	else if(clock_field == CLKSET_MONTH) {
		sDate.Month++;
		if(sDate.Month > 12 ) {		// wrap-around after 12 months
			sDate.Month = 1;
		}
		clock_set();
	}
	else if(clock_field == CLKSET_YEAR) {
		sDate.Year++;
		if(sDate.Year > 99 ) {		// wrap-around after the 2099 XD
			sDate.Year = 25;
		}
		clock_set();
	}
	else if(clock_field == CLKSET_HOUR) {
		sTime.Hours++;
		if(sTime.Hours > 23 ) {		// wrap-around after 23 hours
			sTime.Hours = 0;
		}
		clock_set();
	}
	else if(clock_field == CLKSET_MIN) {
		sTime.Minutes++;
		if(sTime.Minutes > 59 ) {		// wrap-around after 23 hours
			sTime.Minutes = 0;
		}
		clock_set();
	}
	else if(clock_field == CLKSET_DAYNAME) {
		sDate.WeekDay++;
		if(sDate.WeekDay > 7 ) {		// wrap-around after 23 hours
			sDate.WeekDay = 1;
		}
		clock_set();
	}
}
