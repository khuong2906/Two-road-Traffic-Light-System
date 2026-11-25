/*
 * fsm_tuning.c
 *
 *  Created on: Nov 11, 2026
 *      Author: ADMIN
 *
 *  3 che do chinh thoi gian, chuyen bang nut A0:
 *      MAN_RED_MODE -> MAN_YELLOW_MODE -> MAN_GREEN_MODE -> NORMAL_MODE
 *  Trong moi che do:
 *      A1: tang 1 s (qua 99 s quay ve 1 s, khong bao gio bang 0)
 *      A2: luu gia tri dang chinh
 *  Hai duong nhap nhay mau dang chinh (timer6, 0.5 s).
 *  Khi ve NORMAL_MODE: ep RED = GREEN + YELLOW de 2 duong khong lech nhau.
 */

#include "fsm_tuning.h"
#include "global.h"
#include "led_states.h"
#include "button.h"
#include "software_timer.h"
#include "lcd.h"
#include <stdio.h>

static char buffer[30];

/* Tang thoi gian 1 s, vuot 99 s thi quay ve 1 s (khong bao gio bang 0) */
static int increase_duration(int value){
	value += 100;
	if(value > 9900) value = 100;
	return value;
}


void fsm_tuning_run(){
	switch(mode){
		case MAN_RED_MODE:
					sprintf(buffer, "%-8d", buffer_duration_time/100);


					lcd_goto_XY(1, 0);
					lcd_send_string("Mode2: MAN_RED  ");


					lcd_goto_XY(2, 0);
					lcd_send_string("Time    ");


					lcd_goto_XY(2, 8);

					lcd_send_string(buffer);

					if(timer6_flag == 1){
						blinkind_led_traffic_x();
						blinkind_led_traffic_y();
						setTimer6(50);
					}

					if(is_button_pressed(0) == 1){
						clearTimer6();
						status = AUTO_YELLOW;
						status1 = AUTO_YELLOW;
						setTimer6(50);
						buffer_duration_time = duration_time_of_AUTO_YELLOW;
						mode = MAN_YELLOW_MODE;
					}

					if(is_button_pressed(1) == 1){
						buffer_duration_time = increase_duration(buffer_duration_time);

					}

					if(is_button_pressed(2) == 1){
						duration_time_of_AUTO_RED = buffer_duration_time;
					}

			break;
		case MAN_GREEN_MODE:
			sprintf(buffer, "%-8d", buffer_duration_time/100);


			lcd_goto_XY(1, 0);
			lcd_send_string("Mode4: MAN_GREEN");


			lcd_goto_XY(2, 0);
			lcd_send_string("Time    ");


			lcd_goto_XY(2, 8);

			lcd_send_string(buffer);




					if(timer6_flag == 1){
						blinkind_led_traffic_x();
						blinkind_led_traffic_y();
						setTimer6(50);
					}
					if(is_button_pressed(0) == 1){
						clearTimer6();

						// Rang buoc: RED = GREEN + YELLOW, neu khong 2 duong se lech
						// nhau va co the cung Xanh. Luon ep lai truoc khi ve tu dong.
						duration_time_of_AUTO_RED = duration_time_of_AUTO_GREEN + duration_time_of_AUTO_YELLOW;

						status = INIT;
						status1 = INIT1;
						setTimer6(50);

						mode = NORMAL_MODE;





					}
					if(is_button_pressed(1) == 1){
						buffer_duration_time = increase_duration(buffer_duration_time);

					}
					if(is_button_pressed(2) == 1){
						duration_time_of_AUTO_GREEN = buffer_duration_time;
						if(duration_time_of_AUTO_RED != duration_time_of_AUTO_GREEN + duration_time_of_AUTO_YELLOW){
							duration_time_of_AUTO_RED = duration_time_of_AUTO_GREEN + duration_time_of_AUTO_YELLOW;
						}
					}

			break;
		case MAN_YELLOW_MODE:
			sprintf(buffer, "%-8d", buffer_duration_time/100);


			lcd_goto_XY(1, 0);
			lcd_send_string("Mode3:MAN_YELLOW");


			lcd_goto_XY(2, 0);
			lcd_send_string("Time    ");


			lcd_goto_XY(2, 8);

			lcd_send_string(buffer);


			if(timer6_flag == 1){
				blinkind_led_traffic_x();
				blinkind_led_traffic_y();
				setTimer6(50);
			}
			if(is_button_pressed(0) == 1){
				clearTimer6();
				status = AUTO_GREEN;
				status1 = AUTO_GREEN;
				setTimer6(50);
				buffer_duration_time = duration_time_of_AUTO_GREEN;
				mode = MAN_GREEN_MODE;



			}
			if(is_button_pressed(1) == 1){
				buffer_duration_time = increase_duration(buffer_duration_time);

			}
			if(is_button_pressed(2) == 1){
				duration_time_of_AUTO_YELLOW = buffer_duration_time;
			}

			break;
		default:
			break;
	}
}
