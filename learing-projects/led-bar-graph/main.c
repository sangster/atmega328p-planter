// vim: expandtab:sw=4:
#include "main.h"

#define BAUD 9600
#include <util/setbaud.h>

int main(void)
{
    // setup digital OUT
    for (uint8_t i = 0; i < LED_SIZE; ++i) {
        DDRD |= _BV(PIND0 + LED_PINS[i]);
    }

    // setup analog IN
    ADMUX = _BV(REFS0);
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    ADCSRA |= _BV(ADEN);

    for (;;) {
        ADCSRA |= _BV(ADSC);
        loop_until_bit_is_clear(ADCSRA, ADSC);
        map_pot_to_leds(ADC);
    }
}

void map_pot_to_leds(uint16_t reading)
{
    const uint16_t led_level = map_longs(reading, 0, 1023, 0, LED_SIZE - 1) + 1;

    for (uint8_t i = 0; i < LED_SIZE; ++i) {
        uint8_t led_on = reading != 0 && (reading == 1023 || i < led_level);

        if (led_on) {
            PORTD |= _BV(PIND0 + LED_PINS[i]);
        } else {
            PORTD &= ~(_BV(PIND0 + LED_PINS[i]));
        }
    }
}

uint16_t map_longs(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
