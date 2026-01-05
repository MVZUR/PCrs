/*
 * pcrs_delay.c
 *
 *  Created on: Sep 16, 2025
 *      Author: mmazur
 */

#include "main.h"

// non-blocking delay based on SysTick (1kHz freq) [ms]

void myDelay_ms_set(myDelay_t *target, uint32_t delay_time) {
	uint32_t current_tick = HAL_GetTick();
	target->target_tick = current_tick + delay_time;  // modulo 2^32, wrap-around is handled in run()
}

// return 1 when time expired
bool myDelay_ms_run(myDelay_t *target) {
    uint32_t current_tick = HAL_GetTick();
    return ((int32_t)(current_tick - target->target_tick) >= 0);	// no overflow risk
}

