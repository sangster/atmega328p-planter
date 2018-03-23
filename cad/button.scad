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
include <button_vars.scad>;

module button_pin()
{
    difference() {
        cube([button_base_w, button_pin_w*2, button_pin_z]);
        translate([button_pin_w, -1, -1]) {
            cube([button_base_w - button_pin_w*2, button_pin_w+2.1, button_pin_z+2]);
        }
    }
}

module _button()
{
    // Base
    color("MediumPurple") cube([button_base_w, button_base_w, button_base_z]);

    // Button
    translate([button_base_w/2, button_base_w/2, button_base_z-1]) {
        color("DarkSlateGray") cylinder(d = button_d, h = button_h+1, $fn=32);
    }

    color("Gold") {
        translate([0, -1, 0]) {
            button_pin();
            translate([0, button_base_w, 0]) button_pin();
        }
    }
}

module button(part = "positive")
{
    if (part == "negative") {
        translate([0, 0, -button_base_z * 2]) {
            cube([button_base_w, button_base_w, button_base_z * 4]);
        }
    } else {
        _button();
    }
}

button();
