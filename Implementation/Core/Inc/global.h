/*
 * global.h
 *
 *  Created on: Nov 11, 2026
 *      Author: ADMIN
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "software_timer.h"
#include "button.h"
#include "main.h"
#include "fsm_automatic.h"
#include "lcd.h"

#define INIT_SYSTEM			0
#define NORMAL_MODE  		1
#define MAN_RED_MODE 		2
#define MAN_YELLOW_MODE 	3
#define MAN_GREEN_MODE 		4

#define AUTO_RED 	 		11
#define AUTO_GREEN 	 		12
#define AUTO_YELLOW  		13


extern int status;
extern int status1;

#endif /* INC_GLOBAL_H_ */
