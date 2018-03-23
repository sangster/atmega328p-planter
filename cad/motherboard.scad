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
include <motherboard_vars.scad>;

module motherboard(part = "positive")
{
    if (part == "negative") {
        motherboard_pcb_holes();
    } else {
        _motherboard();
    }
}


module _motherboard()
{
    difference() {
        color("Wheat") cube([mb_x, mb_y, mb_z]);
        motherboard_pcb_holes();
    }
}


module motherboard_pcb_holes()
{
    translate([0, 0, -mb_z]) {
        for (location = mb_pcb_holes) {
            translate(location) {
                translate([-mb_pcb_hole_d/2, -mb_pcb_hole_d/2]) {
                    cylinder(d = mb_pcb_hole_d, h = mb_pcb_hole_z, $fn = 32);
                 }
            }
        }
    }
}
