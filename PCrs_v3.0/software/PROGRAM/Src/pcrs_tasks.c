/*
 * tasks.c
 *
 *  Created on: Sep 5, 2025
 *      Author: mmazur
 */

#include "main.h"

// system status variable
extern volatile sys_statuses system_status;

// auto lights variable (flag)
autolights_flag auto_lights = AL_ON;

// stand sensor variable (flag)
standsensor_flag stand_sensor = SS_ON;


// RPM processing variables
volatile uint32_t last_capture = 0;
volatile uint32_t diff_capture = 0;
volatile bool new_period_flag = false;    // ISR set, main clean
uint32_t last_pulse_tick_ms = 0;
volatile float rpm_raw_s = 0.0f;
volatile float rpm_filtered = 0.0f;



// power procedures
void task_power() {

	if(system_status == STATUS_TO_POWER_ON) {
		// wake up CPU & turn on peripherals
		// If CPU has been in Sleep Mode, then wakes up automatically after an interrupt occurs
		control_ADC1_enable();
		//control_FMPI2C1_enable();
		control_I2C1_enable();
		control_I2C2_enable();
		control_I2C3_enable();
		control_SPI1_enable();
		control_SPI2_enable();
		//control_SPI3_enable();
		control_TIM2_enable();
		control_TIM2_it_enable();

		system_status = STATUS_POWER_ON;
	}

	if(system_status == STATUS_TO_SLEEP) {
		// put CPU into Sleep Mode & disable peripherals

		// set low gpio OLED pins to prevent current draw
		//HAL_GPIO_WritePin(GPIOA,CS_oled_Pin,GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(GPIOC,DC_oled_Pin,GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(GPIOC,RESET_oled_Pin,GPIO_PIN_RESET);

		control_ADC1_disable();
		//control_FMPI2C1_disable();
		control_I2C1_disable();
		control_I2C2_disable();
		control_I2C3_disable();
		control_SPI1_disable();
		control_SPI2_disable();
		//control_SPI3_disable();
		control_TIM2_disable();
		control_TIM2_it_disable();

		// CPU Sleep Mode
		HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

		system_status = STATUS_SLEEPING;
	}
}



// RPM counting - ISR Input Capture
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        uint32_t cur = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        uint32_t prev = last_capture;
        last_capture = cur;

        // difference measurement with timer overflow detection
        uint32_t diff = (cur >= prev) ? (cur - prev) : (0xFFFFFFFFu - prev + cur + 1u);
        if (diff == 0) return; // incorrect measurement

        diff_capture = diff;
        new_period_flag = true;
        last_pulse_tick_ms = HAL_GetTick();		// last pulse tick time to count timeout (engine stop case)
    }
}

// diff_capture process function - filtering
void task_process_rpm_from_period(void)
{
    if (!new_period_flag) {
        // check timeout
        uint32_t now_ms = HAL_GetTick();
        if ((now_ms - last_pulse_tick_ms) > (uint32_t)(NO_PULSE_TIMEOUT_S * 1000.0f)) {
            rpm_filtered = 0.0f;
        }
        return;
    }

    uint32_t pd = diff_capture;
    new_period_flag = false;

    // period [s]
    float dt = (float)pd / TIM2_FREQ;

    // sanity check
    const float dt_min = 1.0f / 80.0f;  // ~0.0125 s (9600 RPM)
    const float dt_max = 0.5f;          // ~240 RPM
    if (dt < dt_min || dt > dt_max) {
        return; // ignore other values
    }

    // first measurement - initialization
    if (rpm_raw_s <= 0.0f) {
    	rpm_raw_s = dt;
    } else {
        // process alpha - depends on dt value
        float alpha = 1.0f - expf(-dt / TAU);
        rpm_raw_s += alpha * (dt - rpm_raw_s);
    }

    // convert to RPM: 1 spark = 2 RPM
    rpm_filtered = ENGINE_CYCLE * 60.0f / rpm_raw_s;
}

float task_RPM_Get(void)
{
    return rpm_filtered;
}



// TASKER
// contains all of the tasks that will be repeated in the main loop
void tasker() {
	task_power();
	task_process_rpm_from_period();
}

