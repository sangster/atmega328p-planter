#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

__attribute__((noreturn))
void main(void);

__attribute__((always_inline))
inline void setup_pins();

__attribute__((always_inline))
void usart_init();

uint8_t usart_recv();

__attribute__((always_inline))
void process_command(uint8_t command);

#endif
