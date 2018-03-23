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
include <ui-pcb_vars.scad>;

use <button.scad>;
include <button_vars.scad>;


module ui_pcb(part = "positive")
{
    if (part == "negative") {
        ui_pcb_buttons(part);
        translate([-ui_pcb_button_off_x,
                   -ui_pcb_y + button_base_w + button_pin_w + ui_pcb_button_off_y,
                   -ui_pcb_z]) {
                ui_pcb_holes();
        }
    } else {
        _ui_pcb();
    }
}


module _ui_pcb()
{
    ui_pcb_buttons();

    translate([-ui_pcb_button_off_x,
               -ui_pcb_y + button_base_w + button_pin_w + ui_pcb_button_off_y,
               -ui_pcb_z]) {
        difference() {
            cube([ui_pcb_x, ui_pcb_y, ui_pcb_z]);
            ui_pcb_holes();
        }
    }
}


module ui_pcb_buttons(part = "positive")
{
    for (i = [0:3]) {
        translate([i * (ui_pcb_button_offset), 0]) button(part);
    }
}

module ui_pcb_holes()
{
    for (location = [
            [ui_pcb_hole_tl_x, ui_pcb_hole_tl_y, -ui_pcb_z],
            [ui_pcb_hole_tr_x, ui_pcb_hole_tr_y, -ui_pcb_z],
            [ui_pcb_hole_bl_x, ui_pcb_hole_bl_y, -ui_pcb_z],
            [ui_pcb_hole_br_x, ui_pcb_hole_br_y, -ui_pcb_z]
         ]) {
        translate(location) {
            translate([ui_pcb_hole_d/2, ui_pcb_hole_d/2]) {
                cylinder(d = ui_pcb_hole_d, h =ui_pcb_hole_z, $fn=32);
             }
        }
    }
}
