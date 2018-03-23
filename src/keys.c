#include "keys.h"

static KeyCode parse_keypress(uint16_t reading);


KeyCode get_touch_button(Pinout const keys[])
{
    static uint8_t pressed_key = 0xFF;
    bool is_any_key_pressed = false;

    for (uint8_t i = 0; i < 4; ++i) {
        if (pinout_is_set(keys[i])) {
            is_any_key_pressed = true;

            if (pressed_key != i) {
                pressed_key = i;
                return i + 1;
            } else {
                pressed_key = i;
                return KEY_NONE;
            }
        }
    }

    if (!is_any_key_pressed) {
        pressed_key = 0xFF;
    }
    return KEY_NONE;
}


KeyCode keycode(uint8_t adc_mask)
{
    static KeyCode buffer[5];
    static KeyCode pressed_key = KEY_NONE;
    static KeyCode prev_keypress = KEY_NONE;
    static size_t idx = 0;

    ADMUX = (ADMUX & 0xF0) | adc_mask;     // Select ADC pin
    ADCSRA |= _BV(ADSC);                   // start comparison
    loop_until_bit_is_clear(ADCSRA, ADSC); // wait until conversion complete

    const KeyCode key = parse_keypress(ADC);
    buffer[idx] = key;
    idx = (idx + 1) % 5;

    bool all_same = true;
    for (size_t i = 0; i < 5; ++i) {
        if (buffer[i] != key) {
            all_same = false;
            break;
        }
    }
    if (all_same) {
        pressed_key = key;
    }
    if (pressed_key != prev_keypress) {
        prev_keypress = pressed_key;
        return pressed_key;
    }
    return KEY_NONE;
}


KeyCode parse_keypress(uint16_t reading)
{
    reading /= KEY_READING_DIVISOR;

    if (reading < KEY_BACK_RANGE) return KEY_BACK;
    if (reading < KEY_OK_RANGE)   return KEY_OK;
    if (reading < KEY_DOWN_RANGE) return KEY_DOWN;
    if (reading < KEY_UP_RANGE)   return KEY_UP;

    return KEY_NONE;
}
