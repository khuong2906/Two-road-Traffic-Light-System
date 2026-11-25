/*
 * dht20.c
 *
 *  Created on: Nov 11, 2026
 *      Author: ADMIN
 */



#include "dht20.h"
#include "main.h"
#include "scheduler.h"

extern I2C_HandleTypeDef hi2c1;        // dinh nghia trong main.c

#define DHT20_ADDRESS       (0x38 << 1)
#define DHT20_MEASURE_MS    80
#define DHT20_I2C_TIMEOUT   10          // ms

static int16_t temperature_x10 = 0;
static int16_t humidity_x10 = 0;
static uint8_t valid = 0;

static uint8_t dht20_crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++) {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x31) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

void dht20_trigger(void) {
    uint8_t cmd[3] = {0xAC, 0x33, 0x00};
    if (HAL_I2C_Master_Transmit(&hi2c1, DHT20_ADDRESS, cmd, 3, DHT20_I2C_TIMEOUT) == HAL_OK) {
        SCH_Add_Task(dht20_read, DHT20_MEASURE_MS, 0);     // one-shot sau 80 ms
    }
}

void dht20_read(void) {
    uint8_t d[7];
    if (HAL_I2C_Master_Receive(&hi2c1, DHT20_ADDRESS, d, 7, DHT20_I2C_TIMEOUT) != HAL_OK) return;
    if (d[0] & 0x80) return;                    // cam bien con ban -> bo qua lan nay
    if (dht20_crc8(d, 6) != d[6]) return;       // sai CRC -> bo qua lan nay

    uint32_t raw_h = ((uint32_t)d[1] << 12) | ((uint32_t)d[2] << 4) | (d[3] >> 4);
    uint32_t raw_t = (((uint32_t)d[3] & 0x0F) << 16) | ((uint32_t)d[4] << 8) | d[5];

    /* Tinh bang so nguyen (x10) de khong can float khi in len LCD */
    humidity_x10    = (int16_t)((raw_h * 1000UL) >> 20);
    temperature_x10 = (int16_t)(((raw_t * 2000UL) >> 20) - 500);
    valid = 1;
}

uint8_t dht20_is_valid(void)            { return valid; }
int16_t dht20_get_temperature_x10(void) { return temperature_x10; }
int16_t dht20_get_humidity_x10(void)    { return humidity_x10; }
