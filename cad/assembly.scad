include <box_vars.scad>;

use <box.scad>;
use <box-assembly.scad>;
use <lid-assembly.scad>;
use <ui-pcb.scad>;


translate([box_w, -box_thickness, box_thickness]) {
    rotate([90, 0]) lid_assembly_with_components();
}

translate([]) {
    box_assembly();
}
