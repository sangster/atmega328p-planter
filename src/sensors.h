#ifndef SENSORS_H
#define SENSORS_H
/* @file
 *
 *  "Planter" is a device that control a houseplant's water and light schedules.
 *  Copyright (C) 2018  Jon Sangster
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <sangster/lcd_charmap.h>
#include <sangster/pinout.h>
#include <sangster/ring_buff_8.h>

#include "mhum.h"


/** The currently state reported by the buoy sensor */
enum buoy_level {
    BUOY_UP,      ///< The buoy is floating up, indicating the presence of water
    BUOY_DOWN,    ///< The buoy is not floating, indicating a lack of water
    BUOY_UNKNOWN  ///< The buoy's status hasn't been checked yet
};
typedef enum buoy_level BuoyLevel;

/** A callback called when the buoy's state changes */
typedef void (*BuoyOnChange)(BuoyLevel);


/** The buoy's most recent state */
extern BuoyLevel buoy_state;

/** The most recent humidity reported by the MHUM sensor */
Dht11Fixed16 humidity_f;

/** The most recent temperature reported by the MHUM sensor */
Dht11Fixed16 temperature_f;


/** Poll the buoy for its state */
void read_buoyancy(Pinout, BuoyOnChange);

/** Poll the MHUM sensor for the current moisture % */
void read_moisture(uint8_t adc_mask, RingBuff8*);

/**
 * Poll the MHUM sensor for the current humidity % and tempurature (C)
 *
 * @note Do not check more frequently than 1 Hz.
 */
bool read_humidity_temperature(Pinout, RingBuff8* h, RingBuff8* t);

/**
 * Convert the given sensor values to a user-frinedly string
 *
 * @param [out] dst  Where the string will be placed
 */
void sensors_to_s(char dst[16], uint8_t moisture, uint8_t humidity,
                  uint8_t temperature);
#endif//SENSORS_H
