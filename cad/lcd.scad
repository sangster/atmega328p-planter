include <lcd_vars.scad>;

module lcd_holes()
{
    union() {
        for (x = [lcd_hole_d/2 + lcd_hole_off, lcd_pcb_x - lcd_hole_d/2 - lcd_hole_off]) {
            for (y = [lcd_hole_d/2 + lcd_hole_off, lcd_pcb_y - lcd_hole_d/2 - lcd_hole_off]) {
                translate([x, y, -2]) cylinder(d = lcd_hole_d, h= lcd_pcb_z + 4, $fn=32);
            }
        }
    }
}

module _lcd()
{
    difference() {
        color("OliveDrab") cube([lcd_pcb_x, lcd_pcb_y, lcd_pcb_z]);
        lcd_holes();
    }

    translate([lcd_off_x, lcd_off_y, lcd_pcb_z]) {
        color("LightSkyBlue") cube([lcd_x, lcd_y, lcd_z]);
    }


    translate([lcd_off_x + lcd_x - 1, lcd_off_y + (lcd_y - lcd_backlight_y)/2]) {
        color("Gainsboro") cube([lcd_pcb_x - (lcd_off_x + lcd_x) + 1, lcd_backlight_y, lcd_backlight_z]);
    }

    // Pins
    pins_x = 41.5;
    pins_y =  2.5;
    pins_z = 11.4;

    pins_off_x = 6.5;
    pins_off_y = lcd_pcb_y - 3.5;
    pins_off_z = 3.5 - pins_z;
    translate([pins_off_x, pins_off_y, pins_off_z]) {
        color("Gold") cube([pins_x, pins_y, pins_z]);
    }
}


module lcd(part = "positive")
{
    if (part == "negative") {
        lcd_holes();
    } else {
        _lcd();
    }
}

lcd();
