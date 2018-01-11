#include "mhum.h"

static uint32_t pulse_length(const Pinout pin, PulseState state);



uint8_t map_moisture(uint16_t val)
{
    const uint8_t percent = map_u16(val, MOISTURE_BOTTOM, MOISTURE_TOP, 0, 100);
    return percent < 100 ? percent : 100;
}


void dht11_to_s(char* str, Dht11Fixed16 val)
{
    if (val.fixed.decimal) {
        sprintf(str, "%d.%d", val.fixed.integral, val.fixed.decimal);
    } else {
        itoa(val.fixed.integral, str, 10);
    }
}


/*
 * Note: do not check more frequently than 1 Hz.
 */
bool mhum_read(const Pinout pin, Dht11Fixed16* humid, Dht11Fixed16* temp)
{
    uint8_t data[5];

    pinout_set(pin); // ensure pullup
    _delay_ms(250);

    // issue START
    pinout_make_output(pin);
    pinout_clr(pin);
    _delay_ms(20);

    uint32_t pulse_lengths[80];

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // finish START
        pinout_set(pin);
        _delay_us(40);

        // go back to reading
        pinout_make_pullup_input(pin);
        _delay_us(10);

        if (!pulse_length(pin, LOW)) {
            /* usart_println_P(PSTR("Timed-out waiting for START-low pulse")); */
            return false;
        }
        if (!pulse_length(pin, HIGH)) {
            /* usart_println_P(PSTR("Timed-out waiting for START-high pulse")); */
            return false;
        }

        for (uint8_t i = 0; i < 80; i += 2) {
            pulse_lengths[i + 0] = pulse_length(pin, LOW);
            pulse_lengths[i + 1] = pulse_length(pin, HIGH);
        }
    }

    for (uint8_t i = 0; i < 40; ++i) {
        uint32_t low  = pulse_lengths[i * 2 + 0],
                 high = pulse_lengths[i * 2 + 1];

        if (!low || !high) {
            /* usart_println_P(PSTR("Timed-out waiting data pulse")); */
            return false;
        }

        data[i / 8] <<= 1; // shift in a 0
        if (high > low) {
            data[i / 8] |= 0x01; // make that 0 a 1
        }
    }

    const uint8_t sum = data[0] + data[1] + data[2] + data[3];
    const uint8_t checksum = data[4];

    if (sum == checksum) {
        humid->reading = data[0];
        temp->reading  = data[2];
        return true;
    } else {
        return false;
    }
}


uint32_t pulse_length(const Pinout pin, PulseState state)
{
    static const uint32_t timeout_humid = cycles_per_ms(1000, 1);
    uint32_t len = 0;

    if (state == HIGH) {
        while (pinout_is_set(pin)) {
            if (len++ >= timeout_humid) {
                return 0;
            }
        }
    } else {
        while (pinout_is_clr(pin)) {
            if (len++ >= timeout_humid) {
                return 0;
            }
        }
    }
    return len;
}
