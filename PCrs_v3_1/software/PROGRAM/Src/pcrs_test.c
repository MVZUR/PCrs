/*
 * pcrs_test.c
 *
 *  Created on: May 11, 2026
 *      Author: mmazur
 */
#include "main.h"

extern ADC_HandleTypeDef hadc1;

bool relay_test(relay_test_types test_type) {

	ssd1306_Fill(White);	// display "clear"
	ssd1306_SetCursor(1,1);
	ssd1306_WriteString("Device under test:",Font_7x10,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(500);
	ssd1306_SetCursor(1,14);
	ssd1306_WriteString("Relay test",Font_7x10,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);


	for(uint8_t i=0; i<NUMBER_OF_TESTS; i++) {	// number of tests
		ssd1306_Fill(White);
		ssd1306_SetCursor(1,1);
		switch(test_type) {
			case FULL:
				ssd1306_WriteString("Running FULL test",Font_7x10,Black); break;
			case K1:
				ssd1306_WriteString("Running K1 test",Font_7x10,Black); break;
			case K2:
				ssd1306_WriteString("Running K2 test",Font_7x10,Black); break;
			case K3:
				ssd1306_WriteString("Running K3 test",Font_7x10,Black); break;
			case K4:
				ssd1306_WriteString("Running K4 test",Font_7x10,Black); break;
			case K5:
				ssd1306_WriteString("Running K5 test",Font_7x10,Black); break;
			case K6:
				ssd1306_WriteString("Running K6 test",Font_7x10,Black); break;
		}

		if(i%2==0){
			ssd1306_SetCursor(1,14);
			ssd1306_WriteString("ON",Font_7x10,Black);
		}
		else {
			ssd1306_SetCursor(1,14);
			ssd1306_WriteString("OFF",Font_7x10,Black);
		}

		ssd1306_UpdateScreen();

		if(test_type == FULL || test_type == K1) {
			// K1
			HAL_GPIO_TogglePin(GPIO_Output_relaydrvr_lights_mode_GPIO_Port, GPIO_Output_relaydrvr_lights_mode_Pin);
			ssd1306_SetCursor(27,14);
			ssd1306_WriteString("K1",Font_7x10,Black);
			ssd1306_UpdateScreen();
			HAL_Delay(1000);
		}

		if(test_type == FULL || test_type == K2) {
			// K2
			HAL_GPIO_TogglePin(GPIO_Output_relaydrvr_light_sel_GPIO_Port, GPIO_Output_relaydrvr_light_sel_Pin);
			ssd1306_SetCursor(44,14);
			ssd1306_WriteString("K2",Font_7x10,Black);
			ssd1306_UpdateScreen();
			HAL_Delay(1000);
		}

		if(test_type == FULL || test_type == K3) {
			// K3
			HAL_GPIO_TogglePin(GPIO_Output_relaydrvr_blinkers_together_GPIO_Port, GPIO_Output_relaydrvr_blinkers_together_Pin);
			ssd1306_SetCursor(61,14);
			ssd1306_WriteString("K3",Font_7x10,Black);
			ssd1306_UpdateScreen();
			HAL_Delay(1000);
		}

		if(test_type == FULL || test_type == K4) {
			// K4
			HAL_GPIO_TogglePin(GPIO_Output_relaydrvr_stand_onoff_GPIO_Port, GPIO_Output_relaydrvr_stand_onoff_Pin);
			ssd1306_SetCursor(78,14);
			ssd1306_WriteString("K4",Font_7x10,Black);
			ssd1306_UpdateScreen();
			HAL_Delay(1000);
		}

		if(test_type == FULL || test_type == K5) {
			// K5
			HAL_GPIO_TogglePin(GPIO_Output_relaydrvr_hazard_lights_GPIO_Port, GPIO_Output_relaydrvr_hazard_lights_Pin);
			ssd1306_SetCursor(95,14);
			ssd1306_WriteString("K5",Font_7x10,Black);
			ssd1306_UpdateScreen();
			HAL_Delay(1000);
		}

		if(test_type == FULL || test_type == K6) {
			// K6
			HAL_GPIO_TogglePin(GPIO_Output_relaydrvr_alarm_siren_GPIO_Port, GPIO_Output_relaydrvr_alarm_siren_Pin);
			ssd1306_SetCursor(112,14);
			ssd1306_WriteString("K6",Font_7x10,Black);
			ssd1306_UpdateScreen();
			HAL_Delay(1000);
		}
	}

	//		else {
	//			ssd1306_SetCursor(1,28);
	//			ssd1306_WriteString("No test selected",Font_7x10,Black);
	//			ssd1306_UpdateScreen();
	//			return 1;
	//		}


	ssd1306_Fill(White);
	ssd1306_SetCursor(1,1);
	ssd1306_WriteString("Test complete:",Font_7x10,Black);
	ssd1306_UpdateScreen();
	HAL_Delay(4000);

	return 0;
}



bool photo_sensor_test(void) {

	for(uint8_t i=0; i<NUMBER_OF_TESTS; i++) {
		uint32_t adc_raw=0;
		char text[16];

		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		adc_raw = HAL_ADC_GetValue(&hadc1);

		snprintf(text, sizeof(text), "%lu", adc_raw);

		ssd1306_Fill(White);
		ssd1306_SetCursor(1,1);
		ssd1306_WriteString("Photo sensor test",Font_7x10,Black);
		ssd1306_SetCursor(1,14);
		ssd1306_WriteString(text,Font_7x10,Black);
		ssd1306_UpdateScreen();
		HAL_Delay(500);
	}

return 0;
}

