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
use <box.scad>;
include <box_vars.scad>;

use <button.scad>;
include <button_vars.scad>;

use <lcd.scad>;
include <lcd_vars.scad>;

use <flow.scad>;
include <flow_vars.scad>;

use <switch.scad>;
include <switch_vars.scad>;

use <ui-pcb.scad>;
include <ui-pcb_vars.scad>;

comp_lcd_x = (box_w - lcd_pcb_x) / 2;
comp_lcd_y = box_w - box_w/4 - lcd_y;

comp_ui_pcb_x = comp_lcd_x + 21 - button_base_w/2;
comp_ui_pcb_y = comp_lcd_y - button_base_w - button_pin_w - ui_pcb_button_off_y - 5;

comp_switch_x = comp_lcd_x;
comp_switch_y = comp_lcd_y - switch_plate_y - 5;


module lid_assembly_components()
{
    translate([comp_lcd_x, comp_lcd_y, -lcd_backlight_z]) {
        lcd();
    }

    translate([comp_ui_pcb_x, comp_ui_pcb_y, -button_pin_z]) {
        ui_pcb();
    }

    translate([comp_switch_x, comp_switch_y, -switch_plate_z]) {
        switch();
    }
}

module lid_assembly()
{
    difference() {
        lid();
        lid_assembly_components();

        // LCD screw holes
        translate([comp_lcd_x, comp_lcd_y]) lcd("negative");
        translate([comp_ui_pcb_x, comp_ui_pcb_y, -button_pin_z]) {
            ui_pcb("negative");
        }

        translate([comp_switch_x, comp_switch_y, -switch_plate_z]) {
            switch("negative");
        }
    }
}

module lid_assembly_with_components()
{
    lid_assembly();
    lid_assembly_components();
}


/* lid_assembly_with_components(); */
projection() lid_assembly();
