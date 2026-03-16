#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>

volatile uint8_t interrupt_count = 0;

ISR(INT0_vect)
{
    ++interrupt_count;
}

int main(void)
{
    DDRB = 0xFF;
    PORTD |= (1 << PD2);

    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);
    sei();

    while (1) {
        PORTB = interrupt_count;
    }
}
