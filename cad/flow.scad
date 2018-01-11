include <flow_vars.scad>;

module flow(part = "positive")
{
    if (part == "negative") {
    } else {
        _flow();
    }
}

module _flow()
{
    // Base
    cube([ws_x, ws_y, ws_z]);

    color("Aqua") {
        // Top Tube
        translate([ws_tube_sm_d/2 + ws_wall_off,
                   ws_tube_off_y - ws_tube_sm_d/2, ws_z]) {
            cylinder(d = ws_tube_sm_d, h = ws_tube_h, $fn=32);
        }

        // Bottom Tube
        translate([ws_tube_sm_d/2 + ws_wall_off,
                   ws_tube_off_y - ws_tube_sm_d/2, -ws_tube_h]) {
            cylinder(d = ws_tube_sm_d, h = ws_tube_h, $fn=32);
            hull() {
                translate([0, 0, -ws_tube_bot_sm_len * 3/4]) {
                    cylinder(d = ws_tube_sm_d, h = ws_tube_h, $fn=32);
                }
                translate([0, 0, -ws_tube_bot_sm_len]) {
                    cylinder(d = ws_tube_bg_d, h = ws_tube_h, $fn=32);
                }
            }
        }
    }

    // Wires
    translate([ws_x - ws_wall_off, 0, (ws_z + ws_wire_d*3)/2]) {
        rotate([90, 90]) flow_wires();
    }
}

module flow_wires()
{
    $fn = 32;
    color("Yellow") cylinder(d = ws_wire_d, h = ws_wire_len);
    color("Black") translate([ws_wire_d, 0]) cylinder(d = ws_wire_d, h = ws_wire_len);
    color("Red") translate([ws_wire_d * 2, 0]) cylinder(d = ws_wire_d, h = ws_wire_len);
}
