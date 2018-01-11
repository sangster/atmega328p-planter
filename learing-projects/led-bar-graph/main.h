// vim: expandtab:sw=4:
#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

static const uint8_t LED_PINS[] = {2, 3, 4, 5, 6, 7};
static const uint8_t LED_SIZE = sizeof(LED_PINS) / sizeof(LED_PINS[0]);

__attribute__((OS_main, noreturn))
int main(void);

__attribute__((always_inline))
inline void map_pot_to_leds(const uint16_t reading);

__attribute__((pure, always_inline))
inline uint16_t map_longs(const uint16_t x,
                          const uint16_t in_min,  const uint16_t in_max,
                          const uint16_t out_min, const uint16_t out_max);

#endif
