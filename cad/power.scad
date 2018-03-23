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
include <power_vars.scad>;

module power(part = "positive")
{
    if (part == "negative") {
    } else {
        _power();
    }
}

module _power()
{
    color("DarkSlateGray")
    translate([power_r, power_r])
    hull() {
        $fn = 32;
        cylinder(r = power_r, h = power_z);
        translate([power_y - power_r*2, 0]) cylinder(r = power_r, h = power_z);
        translate([0, power_x - power_r*2]) cylinder(r = power_r, h = power_z);
        translate([power_y - power_r*2, power_x - power_r*2]) cylinder(r = power_r, h = power_z);
    }
}


power();
