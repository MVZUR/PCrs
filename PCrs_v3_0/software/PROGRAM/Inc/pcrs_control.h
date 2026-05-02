/*
 * pcrs_control.h
 *
 *  Created on: Sep 6, 2025
 *      Author: mmazur
 */

#ifndef INC_PCRS_CONTROL_H_
#define INC_PCRS_CONTROL_H_

// system statuses
typedef enum {
    STATUS_WORKING,
	STATUS_TO_POWER_ON,
    STATUS_POWER_ON,
	STATUS_START,
    STATUS_ALERT,
	STATUS_TO_SLEEP,
	STATUS_SLEEPING
} sys_statuses;

#endif /* INC_PCRS_CONTROL_H_ */
