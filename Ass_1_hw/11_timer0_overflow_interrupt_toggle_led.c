#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>

volatile uint8_t overflow_count = 0;

ISR(TIMER0_OVF_vect)
{
    ++overflow_count;
    if (overflow_count >= 100U) {
        PORTB ^= (1 << PB0);
        overflow_count = 0;
    }
}

int main(void)
{
    DDRB |= (1 << PB0);
    TCCR0A = 0x00;
    TCCR0B = (1 << CS02) | (1 << CS00);
    TIMSK0 = (1 << TOIE0);
    sei();

    while (1) {
    }
}
