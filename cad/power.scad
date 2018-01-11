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
