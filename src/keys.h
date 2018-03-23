#ifndef KEYS_H
#define KEYS_H

#include <stdbool.h>
#include <avr/io.h>
#include <sangster/pinout.h>


#define KEY_READING_DIVISOR  20


// NOTE: The physical order of the keys is: [up] [down] [ok] [back]
//       This is in reverse of thir reading value because I accidentally
//       printed my circuit board mirrored, horizontally. Whoops! It still
//       works though, fortunately.
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


KeyCode get_touch_button(Pinout const keys[]);

KeyCode keycode(uint8_t adc_mask);

#endif//KEYS_H
