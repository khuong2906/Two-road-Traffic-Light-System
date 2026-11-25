/*
 * dht20.h
 *
 *  Created on: Nov 11, 2026
 *      Author: ADMIN
 */
#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include <stdint.h>

#define DHT20_PERIOD_MS     2000    /* chu ky doc, khong doc lien tuc */

void    dht20_trigger(void);            /* task dinh ky DHT20_PERIOD_MS */
void    dht20_read(void);               /* task one-shot, do dht20_trigger them vao */

uint8_t dht20_is_valid(void);           /* 1 neu da co it nhat 1 lan doc thanh cong */
int16_t dht20_get_temperature_x10(void);/* nhiet do x10, vd 285 = 28.5 do C */
int16_t dht20_get_humidity_x10(void);   /* do am x10,   vd 652 = 65.2 % */

#endif /* INC_DHT20_H_ */

