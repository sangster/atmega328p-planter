#ifndef KEYS_H
#define KEYS_H
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
#include <stdbool.h>
#include <avr/io.h>
#include <sangster/pinout.h>


/**
 * When one of the 4 physical buttons are pressed, the result is a 10bit value.
 * Dividing that value by a large divisor gives us more consistent results via
 * integer divison.
 */
#define KEY_READING_DIVISOR  20

/**
 * To avoid open/close flicker when the user begins to press a button, we
 * require the same kyepressed to be read multiple times in a row before it
 * registers as a press
 */
#define KEY_DEBOUNCE_DEPTH    5

/**
 * @note The physical order of the keys is: [up] [down] [ok] [back]
 *       This is in reverse of thir reading value because I accidentally
 *       printed my circuit board mirrored, horizontally. Whoops! It still
 *       works though, fortunately.
 */
enum key_code
{
    KEY_NONE,

    KEY_BACK,
#define KEY_BACK_RANGE  2

    KEY_OK,
#define KEY_OK_RANGE    4

    KEY_DOWN,
#define KEY_DOWN_RANGE  6

    KEY_UP,
#define KEY_UP_RANGE    8
};
typedef enum key_code KeyCode;


/**
 * @return The KeyCode of the button currently depressed by the user
 * @retval KEY_NONE No button is depressed
 *
 * @note Keypresses don't repeat: user must release the button before another
 *   keypress will be registered.
 * @note To avoid key "bounce" we require a button to remain pressed
 *   #KEY_DEBOUNCE_DEPTH number of times before registering a keypress
 */
KeyCode keycode(uint8_t adc_mask);

#endif//KEYS_H
