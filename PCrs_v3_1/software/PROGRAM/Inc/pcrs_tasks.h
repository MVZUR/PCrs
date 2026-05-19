/*
 * pcrs_tasks.h
 *
 *  Created on: Sep 5, 2025
 *      Author: mmazur
 */

#ifndef INC_PCRS_TASKS_H_
#define INC_PCRS_TASKS_H_


/* --- RPM definitions --- */
//#define EMA_ALPHA 0.2f   			// EMA filter constant (0..1) - if smaller then signal is smoother
#define TAU 0.08f                	// EMA filter time constant [s] (should be set depending on dynamics)
#define TIM2_FREQ 1000000.0f  		// TIM2 frequency [Hz], default = 1 MHz
#define ENGINE_CYCLE 2         		// 4-stroke = 1 spark per 2 engine cycle
#define NO_PULSE_TIMEOUT_S 1.0f    	// no pulses for defined time [s] => RPM=0


/* --- AUTO LIGHTS --- */
typedef enum {
	AL_OFF,
	AL_ON
} autolights_flag;


/* --- STAND SENSOR --- */
typedef enum {
	SS_OFF,
	SS_ON
} standsensor_flag;


#endif /* INC_PCRS_TASKS_H_ */
