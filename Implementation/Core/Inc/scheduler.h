/*
 * scheduler.h
 *
 *  Created on: Nov 11, 2026
 *      Author: ADMIN
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "stdint.h"
#include <stddef.h>

#define SCH_MAX_TASKS 	41
int __io_putchar(int ch);

extern volatile uint32_t timer_tick;

typedef struct {
void (*pTask)(void); // pointer to the task function
uint32_t Delay; // remaining ticks (relative)
uint32_t Period; // period, 0 = one-shot task
uint8_t RunMe; // flag: task ready to run
uint32_t TaskID;
} sTask;

void SCH_Init(void);
uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t DELAY, uint32_t PERIOD);
uint8_t SCH_Delete_Task(uint32_t taskID);
void SCH_Update(void); // called in the timer interrupt
void SCH_Dispatch_Tasks(void); // called in while(1)





#endif /* INC_SCHEDULER_H_ */
