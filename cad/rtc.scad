include <rtc_vars.scad>;

module rtc(part = "positive")
{
    if (part == "negative") {
    } else {
        _rtc();
    }
}

module _rtc()
{
    color("BurlyWood")
    cube([rtc_x, rtc_y, rtc_z]);

    color("Gold")
    translate([(rtc_x - rtc_pins_x)/2, 0, rtc_z - rtc_pins_z])
    cube([rtc_pins_x, rtc_pins_y, rtc_pins_z]);
}
