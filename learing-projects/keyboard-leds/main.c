// vim: expandtab:sw=4:
#include "main.h"

#define BAUD 9600
#include <util/setbaud.h>

void main(void)
{
    setup_pins();
    usart_init();

    for(;;) {
        process_command(usart_recv());
    }
}

void setup_pins()
{
    DDRD |= _BV(DDD2) | _BV(DDD3) | _BV(DDD4) | _BV(DDD5);
    PORTD &= ~(_BV(PORTD2) | _BV(PORTD3) | _BV(PORTD4) | _BV(PORTD5));
}

void usart_init()
{
    // baud rate
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    UCSR0B = _BV(RXEN0); // enable recv
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01); // frame format: 8bit
}

uint8_t usart_recv()
{
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

void process_command(uint8_t command)
{
    switch (command) {
        case '1':
            PIND |= _BV(PIND2);
            break;
        case '2':
            PIND |= _BV(PIND3);
            break;
        case '3':
            PIND |= _BV(PIND4);
            break;
        case '4':
            PIND |= _BV(PIND5);
            break;
        default:
            // all off
            PORTD &= ~(_BV(PORTD2) | _BV(PORTD3) | _BV(PORTD4) | _BV(PORTD5));
            break;
    }
}
