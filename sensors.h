#ifndef SENSORS_H
#define SENSORS_H
/**
 * @file
 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <sangster/lcd_charmap.h>
#include <sangster/pinout.h>
#include <sangster/ring_buff_8.h>

#include "mhum.h"


/*******************************************************************************
 * Types
 ******************************************************************************/
enum buoy_level {
    BUOY_UP,
    BUOY_DOWN,
    BUOY_NO_READING
};
typedef enum buoy_level BuoyLevel;

typedef void (*BuoyOnChange)(BuoyLevel);


/*******************************************************************************
 * Variables
 ******************************************************************************/
extern BuoyLevel buoy_state;
Dht11Fixed16 humidity_f;
Dht11Fixed16 temperature_f;


/*******************************************************************************
 * Functions
 ******************************************************************************/
void read_buoyancy(Pinout, BuoyOnChange);
void read_moisture(uint8_t adc_mask, RingBuff8*);
bool read_humidity_temperature(Pinout, RingBuff8* h, RingBuff8* t);
void sensors_to_s(char line[16], uint8_t moisture, uint8_t humidity,
                  uint8_t temperature);
#endif//SENSORS_H
