#ifndef LOG_H
#define LOG_H
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
#include <time.h>
#include <avr/pgmspace.h>
#include <sangster/ring_buff_8.h>
#include <sangster/sd.h>

#include "log_period.h"
#include "sensors.h"
#include "settings.h"


#define LOG_STATUS_FILENAME_PATTERN  "%04d-%02d.log" // ex: 2018-02.log
#define LOG_STATUS_HEADER  "Time,Moisture (%),"                                \
                           "Humidity (%),"                                     \
                           "Temperature (C),"                                  \
                           "Time of Last Watering,"                            \
                           "Conf: Lamp Start,"                                 \
                           "Conf: Lamp Period (Hrs),"                          \
                           "Conf: Pump Minimum (% Moisture),"                  \
                           "Conf: Pump Delay (Hrs),"                           \
                           "Conf: Pump Amount (mL),"                           \
                           "Conf: Log Period (Mins),"                          \
                           "Lamp State,"                                       \
                           "Buoy State"

#define LOG_PUMP_FILENAME  "Water.log"
#define LOG_PUMP_HEADER    "Time,"                                             \
                           "Moisture Before,"                                  \
                           "mL"

#define LOG_LAMP_FILENAME  "Lamp.log"
#define LOG_LAMP_HEADER    "Time,"                                             \
                           "New Lamp State"


extern struct tm log_last_time; ///< The last time log_status() was called


/** Log general statistics to #LOG_STATUS_FILENAME_PATTERN */
void log_status(SdFile* dir, struct tm current_time, const Settings* settings,
                uint8_t moisture, uint8_t humidity, uint8_t temperature,
                BuoyLevel buoy_state, bool lamp_is_on,
                struct tm pump_last_time);

/** Log a watering event to #LOG_PUMP_FILENAME */
void log_pump(SdFile* dir, struct tm current_time, uint16_t total_ml,
              uint8_t prev_moisture);

/** Log a change in the Lamp's state to #LOG_LAMP_FILENAME */
void log_lamp(SdFile* dir, struct tm current_time, bool is_on);

/** @return The number of seconds represented by the given LogPeriod */
uint16_t log_period_seconds(LogPeriod);

/** @return The given time plus the given LogPeriod */
struct tm log_next_time(struct tm, LogPeriod);

/** @return A string describing the given LogPeriod */
PGM_P log_period_label_P(LogPeriod);

#endif//LOG_H
