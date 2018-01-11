include <box_vars.scad>;

w = box_w - box_thickness * 2;
wall_off = -w/2 + lid_lip_w + lid_hole_id/2 + lid_hole_space + (lid_hole_od - lid_hole_id)/2;


module column()
{
    difference() {
        // OD
        cylinder(d = col_od, h = col_h, $fn = 16);

        // ID
        translate([0, 0, -1]) cylinder(d = col_id, h = col_h + 2, $fn = 16);

        // Notch
        translate([0, 0, col_h-1]) cylinder(d = col_notch_d, h = col_notch_h + 1, $fn = 16);
    }
}

module shell(height)
{
    off = box_w / 2;
    rad = box_corner_radius;

    hull() {
        translate([-off + rad, -off + rad]) cylinder(r = rad, h = height);
        translate([-off + rad, +off - rad]) cylinder(r = rad, h = height);
        translate([+off - rad, -off + rad]) cylinder(r = rad, h = height);
        translate([+off - rad, +off - rad]) cylinder(r = rad, h = height);
    }
}

module shell_inner()
{
    w = box_w - box_thickness * 2;
    translate([-w/2, -w/2, box_thickness]) cube([w, w, box_h - box_thickness + 1]);
}

module bar()
{
    translate([bar_w/2, wall_space, bar_h - bar_w/2]) {
        rotate([90,0,0]) cylinder(d = bar_w, h = wall_space, $fn=16);
    }
    cube([bar_w, wall_space, bar_h - bar_w / 2]);
}

module fence()
{
    for (i = [0:1:bar_count-1]) {
        translate([i * (bar_w + bar_space), 0]) bar();
    }

    translate ([bar_w/2, 0]) notch();
    translate ([(bar_w + bar_space) * (bar_count - 1), 0]) notch();
}

module notch()
{
    cube([notch_w, notch_len - notch_w/2, notch_h]);
    translate([notch_w/2, notch_len - notch_w/2]) {
        cylinder(r = notch_w/2, h = notch_h, $fn=16);
    }
}

module _box()
{
    difference() {
        shell(box_h);
        shell_inner();
    }

    col_offset = col_od/2 + box_thickness + wall_space;

    for(r = [0:90:359]) {
        rotate([0, 0, r]) {
            // Column
            translate([-box_w/2 + col_offset, +box_w/2 - col_offset]) column();

            // Fence
            translate([-box_w/2 + box_thickness, fence_width/2]) {
                rotate([0, 0, -90]) fence();
            }
        }
    }
}

module lid_holes()
{
    // Holes
    union() {
        for(r = [0:90:359]) {
            rotate([0, 0, r]) {
                translate([wall_off, wall_off, -(box_thickness + 1)]) {
                    // Hole
                    cylinder(d = lid_hole_id, h = 10, $fn=16);
                    // Cone Hole
                    translate([0, 0, 0.9]) {
                        cylinder(d1 = 6, d2 = lid_hole_id, h = 1.8+0.1, $fn=16);
                    }
                }
            }
        }
    }
}

module lid_inner()
{
    w = box_w - box_thickness * 2;
    translate([-w/2, -w/2]) cube([w, w, lid_shell_h - box_thickness + 1]);
}

module lid_up()
{
    difference() {
        union() {
            difference() {
                translate([0, 0, -box_thickness]) shell(lid_shell_h);
                lid_inner();
            }
            for(r = [0:90:359]) {
                rotate([0, 0, r]) {
                    // side-wall
                    translate([-w/2, w/2 - lid_lip_w]) {
                        cube([w, lid_lip_w, lid_lip_h]);
                    }
                    // peg wall
                    translate([wall_off, wall_off]) {
                        cylinder(d = lid_hole_od, h = lid_lip_h, $fn=16);
                    }
                }
            }
        }
        lid_holes();
    }
}

module box()
{
    translate([box_w/2, box_w/2]) _box();
}

module lid(part = "positive")
{
    if (part == "negative") {
        lid_holes();
    } else {
        translate([box_w/2 - box_thickness, box_w/2 - box_thickness]) {
            rotate([180, 0]) lid_up();
        }
    }
}

/*******************************************************************************
 * Main
 ******************************************************************************/
box();
/* lid_up(); */
/* rotate([0, 90]) { */
/*     translate([0, 0, lid_shell_h*10]) rotate([180, 0]) lid_up(); */

/*     translate([0, 0, -box_h]) box(); */
/* } */
