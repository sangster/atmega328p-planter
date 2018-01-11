// vim: expandtab:sw=4:
#include <avr/io.h>
#include <Arduino.h>

const int DARK_FLOOR = 500;

const int IN_PHOTOCELL = A0;
const int OUT_LED = 2;

int main (void)
{
    init();
    pinMode(OUT_LED, OUTPUT);

    for(;;) {
        int brightness = analogRead(IN_PHOTOCELL);
        digitalWrite(OUT_LED, brightness < DARK_FLOOR ? HIGH : LOW);
        _delay_ms(1);
    }

    return 0;
}
