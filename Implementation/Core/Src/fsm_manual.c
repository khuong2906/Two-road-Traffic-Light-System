/*
 * fsm_manual.c
 *
 *  Created on: Nov 11, 2026
 *      Author: ADMIN
 */

#include "fsm_manual.h"
#include "global.h"
#include "led_states.h"
#include "button.h"
#include "main.h"
#include "fsm_automatic.h"
#include "fsm_tuning.h"
#include "dht20.h"
#include <stdio.h>

char buffer1[30];
char buffer2[30];

/* ---------- Che do thu cong ----------
 * 4 trang thai an toan, di theo dung thu tu Xanh -> Vang -> Do:
 *   0: Duong1 DO   - Duong2 XANH
 *   1: Duong1 DO   - Duong2 VANG
 *   2: Duong1 XANH - Duong2 DO
 *   3: Duong1 VANG - Duong2 DO
 * Khong co buoc nao 2 duong cung Xanh. */
static int manual_step = 0;

static void manual_apply(int step){
	switch(step){
		case 0: state_red();    state_green1();  status = AUTO_RED;    status1 = AUTO_GREEN;  break;
		case 1: state_red();    state_yellow1(); status = AUTO_RED;    status1 = AUTO_YELLOW; break;
		case 2: state_red1();   state_green();   status = AUTO_GREEN;  status1 = AUTO_RED;    break;
		case 3: state_red1();   state_yellow();  status = AUTO_YELLOW; status1 = AUTO_RED;    break;
		default: break;
	}
}

/* Tim buoc thu cong tuong ung voi den dang sang, de khi vao che do
 * thu cong den giu nguyen, khong nhay dot ngot tu Xanh sang Do. */
static int manual_step_from_status(void){
	if(status == AUTO_RED    && status1 == AUTO_YELLOW) return 1;
	if(status == AUTO_GREEN  && status1 == AUTO_RED)    return 2;
	if(status == AUTO_YELLOW && status1 == AUTO_RED)    return 3;
	return 0;
}


void fsm_manual_run(){
	switch(mode){
		case INIT_SYSTEM:
			  button_init();
			  status = INIT;
			  status1 = INIT1;
			  setTimer0(25);
			  setTimer6(50);

			  mode = NORMAL_MODE;


			break;
		case NORMAL_MODE:
				  // Dong 1: "Lane 1:4  T28.5C"   Dong 2: "Lane 2:2  H65%  "
				  if(dht20_is_valid()){
					  int t = dht20_get_temperature_x10();
					  int h = dht20_get_humidity_x10();
					  const char *sign = (t < 0) ? "-" : "";
					  if(t < 0) t = -t;
					  sprintf(buffer1, "%-3dT%s%d.%dC   ", timer1_counter/100, sign, t/10, t%10);
					  sprintf(buffer2, "%-3dH%d%%     ", timer2_counter/100, (h + 5)/10);
				  } else {
					  sprintf(buffer1, "%-3dT--.-C  ", timer1_counter/100);
					  sprintf(buffer2, "%-3dH--%%    ", timer2_counter/100);
				  }
				  buffer1[9] = '\0';     // phan sau "Lane x:" chi con 9 o
				  buffer2[9] = '\0';


				  lcd_goto_XY(1, 0);
				  lcd_send_string("Lane 1:");


				  lcd_goto_XY(1, 7);
				  lcd_send_string(buffer1);


				  lcd_goto_XY(2, 0);
				  lcd_send_string("Lane 2:");


				  lcd_goto_XY(2, 7);
				  lcd_send_string(buffer2);



				  fsm_automatic_run();
				  fsm_automatic_run1();


					if(is_button_pressed(0) == 1){
						// Dung 2 timer tu dong, giu nguyen den dang sang
						clearTimer1();
						clearTimer2();
						manual_step = manual_step_from_status();
						manual_apply(manual_step);
						mode = MANUAL_MODE;
					}

			break;
		case MANUAL_MODE:
			lcd_goto_XY(1, 0);
			lcd_send_string("Mode1: MANUAL   ");

			lcd_goto_XY(2, 0);
			lcd_send_string("A1: next light  ");

			// A1: sang trang thai ke tiep
			if(is_button_pressed(1) == 1){
				manual_step = (manual_step + 1) % 4;
				manual_apply(manual_step);
			}

			// A0: sang che do chinh thoi gian Do
			if(is_button_pressed(0) == 1){
				status = AUTO_RED;
				status1 = AUTO_RED;
				state_red();
				state_red1();
			    setTimer6(50);

			    buffer_duration_time = duration_time_of_AUTO_RED;

			    mode = MAN_RED_MODE;
			}
			break;
		case MAN_RED_MODE:
		case MAN_YELLOW_MODE:
		case MAN_GREEN_MODE:
			fsm_tuning_run();       // 3 che do chinh thoi gian nam trong fsm_tuning.c
			break;
		default:
			break;
	}
}
