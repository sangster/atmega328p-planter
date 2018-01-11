include <switch_vars.scad>;

module switch_holes()
{
    h = switch_plate_z + switch_base_z + switch_handle_z;
    off_x = switch_plate_x / 2;
    off_y = switch_hole_d/2 + switch_hole_off;
    translate([off_x, 0, -h/2]) {
        translate([0, off_y])
            cylinder(d = switch_hole_d, h = h, $fn=32);
        translate([0, switch_plate_y - off_y])
            cylinder(d = switch_hole_d, h = h, $fn=32);
    }
}

module _switch()
{
    // Handle
    h_off_x = (switch_plate_x - switch_handle_x) / 2;
    h_off_y = (switch_plate_y - switch_handle_y) / 2;
    translate([h_off_x, h_off_y, 0]) {
        color("Green")
            cube([switch_handle_x, switch_handle_y, switch_handle_z]);
    }
    // Plate
    cube([switch_plate_x, switch_plate_y, switch_plate_z]);

    // Base
    b_off_y = (switch_plate_y - switch_base_y) / 2;
    translate([0, b_off_y, -switch_base_z]) {
        color("DarkSlateGray")
            cube([switch_base_x, switch_base_y, switch_base_z]);
    }
}

module switch(part = "positive")
{
    if (part == "negative") {
        switch_holes();
    } else {
        difference() {
            _switch();
            switch_holes();
        }
    }
}

switch();
