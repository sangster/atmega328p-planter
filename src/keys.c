/* @file
 *
 *  "Planter" is a device that control a houseplant's water and light schedules.
 *  Copyright (C) 2018  Jon Sangster
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "keys.h"

static KeyCode parse_keypress(uint16_t reading);


KeyCode keycode(uint8_t adc_mask)
{
    static KeyCode buffer[KEY_DEBOUNCE_DEPTH];
    static KeyCode pressed_key = KEY_NONE;
    static KeyCode prev_keypress = KEY_NONE;
    static size_t idx = 0;

    ADMUX = (ADMUX & 0xF0) | adc_mask;     // Select ADC pin
    ADCSRA |= _BV(ADSC);                   // start comparison
    loop_until_bit_is_clear(ADCSRA, ADSC); // wait until conversion complete

    const KeyCode key = parse_keypress(ADC);
    buffer[idx] = key;
    idx = (idx + 1) % KEY_DEBOUNCE_DEPTH;

    bool all_same = true;
    for (size_t i = 0; i < KEY_DEBOUNCE_DEPTH; ++i) {
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
