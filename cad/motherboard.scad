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
