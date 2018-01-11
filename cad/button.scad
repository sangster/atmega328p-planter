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
