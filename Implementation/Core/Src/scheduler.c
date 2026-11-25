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

static uint8_t  SCH_count = 0;          // so task dang co trong danh sach
static uint32_t SCH_next_id = 1;        // ID tiep theo se cap (0 = khong hop le)

sTask SCH_tasks_G[SCH_MAX_TASKS];

/*
//int timer_tick_count = 0;
int global_tick_count = 0;  // Thêm biến để theo dõi tổng số tick kể từ khi hệ thống bắt đầu
*/


static uint8_t SCH_Insert(void (*pFunc)(void), uint32_t delay, uint32_t period, uint32_t id) {
    if (SCH_count >= SCH_MAX_TASKS) return 0;

    /* Tim vi tri: tru dan Delay cua cac task phia truoc.
     * Dung >= de task cung thoi diem chay theo thu tu them vao. */
    uint8_t i = 0;
    while (i < SCH_count && delay >= SCH_tasks_G[i].Delay) {
        delay -= SCH_tasks_G[i].Delay;
        i++;
    }

    /* Dich cac task phia sau sang phai 1 o */
    for (uint8_t j = SCH_count; j > i; j--) {
        SCH_tasks_G[j] = SCH_tasks_G[j - 1];
    }

    SCH_tasks_G[i].pTask  = pFunc;
    SCH_tasks_G[i].Delay  = delay;
    SCH_tasks_G[i].Period = period;
    SCH_tasks_G[i].RunMe  = (i == 0 && delay == 0);   // chi task DAU moi duoc danh dau den han
    SCH_tasks_G[i].TaskID = id;
    SCH_count++;

    /* Task ngay sau gio tinh tuong doi so voi task vua chen */
    if (i + 1 < SCH_count) {
        SCH_tasks_G[i + 1].Delay -= delay;
    }
    return 1;
}

/* Xoa task o vi tri index, cong Delay cua no cho task phia sau. */
static void SCH_Remove_At(uint8_t index) {
    if (index + 1 < SCH_count) {
        SCH_tasks_G[index + 1].Delay += SCH_tasks_G[index].Delay;
    }
    for (uint8_t j = index; j + 1 < SCH_count; j++) {
        SCH_tasks_G[j] = SCH_tasks_G[j + 1];
    }
    SCH_count--;
    SCH_tasks_G[SCH_count].pTask = 0;

    /* Task moi len dau danh sach co Delay = 0 tuc la da den han */
    if (index == 0 && SCH_count > 0 && SCH_tasks_G[0].Delay == 0) {
        SCH_tasks_G[0].RunMe = 1;
    }
}

//API
uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD) {
    if (pFunction == 0) return SCH_NO_TASK_ID;

    uint32_t delay  = DELAY / TICK;
    uint32_t period = PERIOD / TICK;
    if (PERIOD > 0 && period == 0) period = 1;   // chu ky nho hon 1 tick -> 1 tick

    __disable_irq();
    uint32_t id = SCH_next_id;
    if (!SCH_Insert(pFunction, delay, period, id)) {
        __enable_irq();
        return SCH_NO_TASK_ID;                   // danh sach day
    }
    SCH_next_id++;
    if (SCH_next_id == SCH_NO_TASK_ID) SCH_next_id = 1;
    __enable_irq();
    return id;
}

void SCH_Update(void) {
    if (SCH_count == 0) return;

    if (SCH_tasks_G[0].Delay > 0) {
        SCH_tasks_G[0].Delay--;
        if (SCH_tasks_G[0].Delay == 0) SCH_tasks_G[0].RunMe = 1;
    } else {
        /* Task dau da den han nhung chua duoc dispatch (task truoc chay lau):
         * van dem thoi gian cho task thu 2 de no khong bi tre lich. */
        SCH_tasks_G[0].RunMe = 1;
        if (SCH_count > 1 && SCH_tasks_G[1].Delay > 0) SCH_tasks_G[1].Delay--;
    }
}

void SCH_Dispatch_Tasks(void) {

    while (1) {
        __disable_irq();
        if (SCH_count == 0 || SCH_tasks_G[0].RunMe == 0) {
            __enable_irq();
            return;
        }

        sTask task = SCH_tasks_G[0];
        SCH_Remove_At(0);

        /* Task tuan hoan: them lai voi Delay = Period (giu nguyen ID).
         * Them lai TRUOC khi chay de task co the tu xoa chinh no. */
        if (task.Period > 0) {
            SCH_Insert(task.pTask, task.Period, task.Period, task.TaskID);
        }

        __enable_irq();

        (*task.pTask)();        // one-shot (Period = 0) khong duoc them lai -> tu bi xoa
    }
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID == SCH_NO_TASK_ID) return 0;
    __disable_irq();

    for (uint8_t i = 0; i < SCH_count; i++) {
        if (SCH_tasks_G[i].TaskID == taskID) {
            SCH_Remove_At(i);
            __enable_irq();
            return 1;
        }
    }
    __enable_irq();
    return 0;                                    // khong tim thay
}

void SCH_Init(void){
	 __disable_irq();
    for (int i = 0; i < SCH_MAX_TASKS; i++) {
//    	SCH_Delete_Task(i);

        SCH_tasks_G[i].pTask = 0;
        SCH_tasks_G[i].Delay = 0;
        SCH_tasks_G[i].Period = 0;
        SCH_tasks_G[i].RunMe = 0;
        SCH_tasks_G[i].TaskID = 0;
        SCH_tasks_G[i].TaskID = SCH_NO_TASK_ID;
    }
    SCH_count = 0;
    SCH_next_id = 1;
    __enable_irq();
}
