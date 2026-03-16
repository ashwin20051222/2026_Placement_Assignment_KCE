#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t safety_shutdown = 0;

ISR(INT0_vect)
{
    safety_shutdown = 1;
}

int main(void)
{
    DDRB |= (1 << PB0) | (1 << PB1);
    DDRB &= (uint8_t)~(1 << PB2);
    PORTB |= (1 << PB2);
    PORTD |= (1 << PD2);

    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);
    sei();

    while (1) {
        if (safety_shutdown != 0U) {
            PORTB &= (uint8_t)~(1 << PB0);
            PORTB |= (1 << PB1);
        } else {
            PORTB |= (1 << PB0);
            PORTB &= (uint8_t)~(1 << PB1);
        }

        if ((PINB & (1 << PB2)) == 0U) {
            _delay_ms(20);
            if ((PINB & (1 << PB2)) == 0U) {
                safety_shutdown = 0;
            }
        }
    }
}
