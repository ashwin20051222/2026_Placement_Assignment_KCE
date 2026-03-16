#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>

volatile uint8_t event_counter = 0;

ISR(INT0_vect)
{
    if ((PIND & (1 << PD2)) != 0U) {
        PORTB |= (1 << PB0);
    } else {
        PORTB &= (uint8_t)~(1 << PB0);
    }

    PORTB ^= (1 << PB1);
    ++event_counter;
}

int main(void)
{
    DDRB |= (1 << PB0) | (1 << PB1);
    DDRC = 0x3F;

    EICRA = (1 << ISC00);
    EIMSK = (1 << INT0);
    sei();

    while (1) {
        PORTC = event_counter & 0x3F;
    }
}
