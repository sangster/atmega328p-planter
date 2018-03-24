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
#include "rtc.h"


void lcd_show_time(Lcd* lcd, uint8_t row, struct tm current_time)
{
    char rtc_string[25], line[14];

    current_time.tm_sec = 0;

    asctime_r(&current_time, rtc_string);

    strncpy(line, &rtc_string[4], 6);
    line[6] = ',';
    line[7] = ' ';
    strncpy(&line[8], &rtc_string[11], 5);
    line[13] = '\0';

    lcd_buffer_update(lcd, row, line);
}
