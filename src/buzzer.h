#ifndef BUZZER_H
#define BUZZER_H
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
#include <avr/interrupt.h>
#include <util/delay.h>
#include <sangster/pinout.h>
#include <sangster/timer0.h>


/*******************************************************************************
 * Constants
 ******************************************************************************/
#define NOTE_D0 98
#define NOTE_D1 294
#define NOTE_D2 330
#define NOTE_D3 350
#define NOTE_D4 393
#define NOTE_D5 441
#define NOTE_D6 495
#define NOTE_D7 556
#define NOTE_DL1 147
#define NOTE_DL2 165
#define NOTE_DL3 175
#define NOTE_DL4 196
#define NOTE_DL5 221
#define NOTE_DL6 248
#define NOTE_DL7 278
#define NOTE_DH1 589
#define NOTE_DH2 661
#define NOTE_DH3 700
#define NOTE_DH4 786
#define NOTE_DH5 882
#define NOTE_DH6 990
#define NOTE_DH7 112

#define WHOLE_MILLIS 1060

#define WHOLE 1
#define HALF 0.5
#define QUARTER 0.25
#define EIGHTH 0.125
#define SIXTEENTH 0.625


extern const uint16_t tune[];  ///< The notes that comprise the alarm tune
extern const float duration[]; ///< The duration of each note in tune[]

extern bool buzzer_is_on; ///< If the buzzer is currently active


/**
 * Turn the buzzer on or off.
 *
 * This only turns it on. You still need to call buzzer_sound() in response to
 * the #TIMER2_COMPA_vect inturrupt to product the correct notes.
 */
void buzzer_set_enabled(bool);

/**
 * Turns the buzzer on or off at the correct times to product the correct
 * frequency of the curent note in the tune[].
 *
 * This function must be called in response to the #TIMER2_COMPA_vect interrupt.
 *
 * @param pin The pin the buzzer is connected to
 */
void buzzer_sound(Pinout);

#endif//BUZZER_H
