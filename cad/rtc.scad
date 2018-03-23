/*
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
include <rtc_vars.scad>;

module rtc(part = "positive")
{
    if (part == "negative") {
    } else {
        _rtc();
    }
}

module _rtc()
{
    color("BurlyWood")
    cube([rtc_x, rtc_y, rtc_z]);

    color("Gold")
    translate([(rtc_x - rtc_pins_x)/2, 0, rtc_z - rtc_pins_z])
    cube([rtc_pins_x, rtc_pins_y, rtc_pins_z]);
}
