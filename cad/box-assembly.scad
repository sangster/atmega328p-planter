use <box.scad>;
include <box_vars.scad>;

use <button.scad>;
include <button_vars.scad>;

use <lcd.scad>;
include <lcd_vars.scad>;

use <flow.scad>;
include <flow_vars.scad>;

use <motherboard.scad>;
include <motherboard_vars.scad>;

use <power.scad>;
include <power_vars.scad>;

use <rtc.scad>;
include <rtc_vars.scad>;

use <switch.scad>;
include <switch_vars.scad>;

module _box_assembly()
{
    // Power
    translate([box_w - box_thickness - wall_space - power_y - mb_z - 5,
               box_h - box_thickness - power_x,
               box_w - box_thickness - wall_space - power_z]) {
        rotate([0, 0, 0]) power();
    }

    // Water Flow Sensor
    translate([wall_space * 4, box_h - ws_tube_sm_d/2, box_w - ws_z - 20]) {
        rotate([0, 0, -90]) flow();
    }

    // RTC
    /* translate([box_w - box_thickness - wall_space - power_z, */
    /*            box_h - box_thickness - rtc_y, */
    /*            box_w - box_thickness - wall_space - rtc_z]) { */
    translate([box_w - box_thickness - wall_space - mb_z,
               box_h - box_thickness,
               rtc_z + box_thickness + wall_space]) {
        rotate([0, 180, 90]) rtc();
    }

    // Motherboard
    translate([box_w - box_thickness - wall_space,
               mb_x,
               box_w - box_thickness - wall_space - col_od - mb_y]) {
        rotate([90, 0, -90]) motherboard();
    }

    // Box
    % translate([0, box_h]) rotate([90, 0]) box();
}

module box_assembly(part = "positive")
{
    if (part == "negative") {
    } else {
        _box_assembly();
    }
}

box_assembly();
