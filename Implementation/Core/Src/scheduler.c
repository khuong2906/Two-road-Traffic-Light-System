/*
 * scheduler.c
 *
 *  Created on: Nov 11, 2026
 *      Author: ADMIN
 */

#include "scheduler.h"
#include "main.h"               // cho __disable_irq / __enable_irq
#include <stdlib.h>
#include <stdio.h>

volatile uint32_t timer_tick_count;

#define TICK 10


sTask SCH_tasks_G[SCH_MAX_TASKS];
uint8_t current_index_task = 0;

//int timer_tick_count = 0;
int global_tick_count = 0;  // Thêm biến để theo dõi tổng số tick kể từ khi hệ thống bắt đầu

uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD) {
    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
        if (SCH_tasks_G[i].pTask == 0) {
            SCH_tasks_G[i].pTask  = pFunction;
            SCH_tasks_G[i].Delay  = DELAY / TICK;
            SCH_tasks_G[i].Period = PERIOD / TICK;
            SCH_tasks_G[i].RunMe  = 0;
            SCH_tasks_G[i].TaskID = i;
            return i;
        }
    }
    return SCH_MAX_TASKS;
}

void SCH_Update(void) {
    timer_tick_count++;
    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
        sTask *t = &SCH_tasks_G[i];
        if (t->pTask == 0) continue;

        if (t->Delay > 0) t->Delay--;           // giam truoc, kiem tra sau

        if (t->Delay == 0) {
            if (t->Period) {                    // periodic
                t->RunMe++;
                t->Delay = t->Period;           // chu ky dung bang Period tick
            } else if (t->RunMe == 0) {         // one-shot: chi bao 1 lan
                t->RunMe = 1;
            }
        }
    }
}

void SCH_Dispatch_Tasks(void) {
    for (uint32_t i = 0; i < SCH_MAX_TASKS; i++) {
        if (SCH_tasks_G[i].pTask && SCH_tasks_G[i].RunMe > 0) {
            __disable_irq();
            SCH_tasks_G[i].RunMe--;
            __enable_irq();

            (*SCH_tasks_G[i].pTask)();
            printf("Task %d executed at %d TICK\n\r", SCH_tasks_G[i].TaskID, global_tick_count);

            if (SCH_tasks_G[i].Period == 0) {
                printf("Task %d deleted at %d TICK\n\r", SCH_tasks_G[i].TaskID, global_tick_count);// one-shot: xoa, giai phong slot
                SCH_Delete_Task(i);
            }
        }
    }
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID >= SCH_MAX_TASKS) return 0;
    uint8_t ok = (SCH_tasks_G[taskID].pTask != 0);
    SCH_tasks_G[taskID].pTask  = 0;
    SCH_tasks_G[taskID].Delay  = 0;
    SCH_tasks_G[taskID].Period = 0;
    SCH_tasks_G[taskID].RunMe  = 0;
    SCH_tasks_G[taskID].TaskID = 0;
    return ok;
    printf("Current index task after delete: %d\n\r", current_index_task);
}

void SCH_Init(void){
    current_index_task = 0;
    for (int i = 0; i < SCH_MAX_TASKS; i++) {
//    	SCH_Delete_Task(i);

        SCH_tasks_G[i].pTask = 0;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;
        SCH_tasks_G[i].TaskID = 0;
    }
}
