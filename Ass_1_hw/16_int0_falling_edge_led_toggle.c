#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>

ISR(INT0_vect)
{
    PORTB ^= (1 << PB0);
}

int main(void)
{
    DDRB |= (1 << PB0);
    PORTD |= (1 << PD2);

    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);
    sei();

    while (1) {
    }
}
