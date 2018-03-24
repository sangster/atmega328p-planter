#ifndef MHUM_H
#define MHUM_H
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
#include <util/atomic.h>
#include <util/delay.h>
#include <sangster/util.h>
#include <sangster/pinout.h>
#include <sangster/ring_buff_8.h>
#include <stdbool.h>
#include <stdfix.h>
#include <stdio.h>


/** MHUM Calibration: The 10-bit sensor value that represents 0% */
#define MOISTURE_BOTTOM    0

/** MHUM Calibration: The 10-bit sensor value that represents 100% */
#define MOISTURE_TOP     650


/**
 * A fixed-point decimal number represented by an 8-bit integer part and an
 * 8-bit decimal part
 */
union dht11_fixed_16
{
    uint16_t reading;
    struct
    {
        uint8_t integral;
        uint8_t decimal; //< @note seems to always be 0x00 with the DHT11
    } fixed;
};
typedef union dht11_fixed_16 Dht11Fixed16;


/**
 * Reads the current humidity and temperature from the MHUM sensor.
 *
 * @param [in]  pin    The pin the sensor is connected to
 * @param [out] humid  Destination for the humidity
 * @param [out] temp   Destination for the tempurature
 *
 * @note Do not check more frequently than 1 Hz.
 */
bool mhum_read(const Pinout, Dht11Fixed16* humid, Dht11Fixed16* temp);

/**
 * Converts the given number to a decimal string.
 *
 * @param [out] str  The string's destination
 */
void dht11_to_s(char*, Dht11Fixed16);

/** @return the moisture % represented by the given 10-bit value */
uint8_t map_moisture(uint16_t);

#endif//MHUM_H
