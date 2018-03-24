#ifndef PUMP_H
#define PUMP_H
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
#include <time.h>
#include <sangster/pinout.h>
#include <sangster/timer0.h>
#include <sangster/usart_p.h>

#include "sensors.h"
#include "settings.h"

/**
 * If the flow sensor doesn't detect any water moving after this many
 * milliseconds, it probably has become unplugged somehow
 */
#define PUMP_UNPLUGGED_CHECK_DELAY  2000

/**
 * The YF-S402 flow sensor returns a frequency 73 times times greater than the
 * flow rate, in L/min.
 */
#define YF_S402_FREQUENCY_FACTOR    73.0

/** Sentinel value returned by pump_check() when it's still pumping */
#define PUMP_STATUS_PUMPING         -1

/** Sentinel value returned by pump_check() when the sensor is unplugged */
#define PUMP_STATUS_UNPLUGGED       -2


/** Used to count the current flow sensor frequency */
volatile uint8_t pump_count;

/** The total water pumped so far, during this watering action */
double pump_total_ul;

/** The timer0 counter when the pump started */
uint16_t pump_started_at;

/** The date/time when the plant was last watered */
extern struct tm pump_last_time;


/**
 * @param [in] pin  The pin the pump is connected to
 */
void pump_start(const Pinout);

/**
 * Check if the pump should be stopped because it has already pumped enough
 * water.
 *
 * @return The total mL pumped. `-1` if the pump is still going or `-2` if it
 *   is unplugged
 * @note This function should be called on each main application loop.
 */
int16_t pump_check(Pinout, const Settings*, BuoyLevel);


/**
 * Increments the frequency counter when a signal is received from the sensor.
 *
 * @note Should be called in response to INT0_vect or INT1_vect, depending on
 *   which pin the sensor is connected to.
 */
__attribute__((always_inline))
static inline void pump_flow_callback()
{
    ++pump_count;
}


/** Reset the pump in preperation of another pump action */
__attribute__((always_inline))
static inline void pump_reset()
{
    pump_count = 0;
    pump_total_ul = 0;
}

#endif//PUMP_H
