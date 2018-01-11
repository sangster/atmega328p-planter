/* vim: set expandtab:sw=4:*/
#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>

void toggleLED(int LEDPin);

int main (void)
{
    pinMode(LED_BUILTIN, OUTPUT);

    while(1) {
        digitalWrite(LED_BUILTIN, HIGH);
        /* delay(500); // TODO: Why doesn't this work? (light is always on) */
        _delay_ms(1000);

        digitalWrite(LED_BUILTIN, LOW);
        _delay_ms(3000);
    }

    return 0;
}

void toggleLED(int LEDPin)
{
    digitalWrite(LEDPin, !digitalRead(LEDPin));
}
