/*
 * software_timer.h
 *
 *  Created on: Nov 11, 2026
 *      Author: ADMIN
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_
extern int timer0_flag, timer1_flag, timer2_flag, timer3_flag, timer4_flag, timer5_flag, timer6_flag;
extern int timer0_counter, timer1_counter, timer2_counter, timer3_counter, timer4_counter, timer5_counter, timer6_counter;



void setTimer0(int duration);
void setTimer1(int duration);
void setTimer2(int duration);
void setTimer3(int duration);
void setTimer4(int duration);
void setTimer5(int duration);
void setTimer6(int duration);


void clearTimer0();
void clearTimer1();
void clearTimer2();
void clearTimer3();
void clearTimer4();
void clearTimer5();
void clearTimer6();

void timer_run();
#endif /* INC_SOFTWARE_TIMER_H_ */
