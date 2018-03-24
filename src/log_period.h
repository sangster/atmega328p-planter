#ifndef LOG_PERIOD_H
#define LOG_PERIOD_H
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

/** The user's options for how frequently statistics are logged.  */
enum log_period
{
#define LOG_PERIOD_FIRST  LOG_5_MINS
    LOG_5_MINS,
    LOG_10_MINS,
    LOG_15_MINS,
    LOG_30_MINS,
    LOG_1_HOUR,
    LOG_2_HOURS,
    LOG_3_HOURS,
    LOG_4_HOURS,
    LOG_6_HOURS,
    LOG_8_HOURS,
    LOG_12_HOURS,
    LOG_24_HOURS,
    LOG_48_HOURS,
#define LOG_PERIOD_LAST  LOG_48_HOURS
};
typedef enum log_period LogPeriod;

#endif//LOG_PERIOD_H
