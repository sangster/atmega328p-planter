#ifndef RTC_H
#define RTC_H
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

#include <time.h>
#include <avr/io.h>
#include <sangster/lcd.h>
#include <sangster/rtc_1307.h>

/**
 * Show the given time on the LCD screen
 * @param [in] row  The LCD row to print the time on
 */
void lcd_show_time(Lcd*, uint8_t row, struct tm);

#endif//RTC_H
