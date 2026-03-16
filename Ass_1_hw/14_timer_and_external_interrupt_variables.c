#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>

volatile uint8_t timer_events = 0;
volatile uint8_t ext_events = 0;

ISR(TIMER0_OVF_vect)
{
    ++timer_events;
}

ISR(INT0_vect)
{
    ++ext_events;
}

int main(void)
{
    DDRB = 0xFF;
    DDRC = 0x3F;
    PORTD |= (1 << PD2);

    TCCR0A = 0x00;
    TCCR0B = (1 << CS02) | (1 << CS00);
    TIMSK0 = (1 << TOIE0);

    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);
    sei();

    while (1) {
        PORTB = timer_events;
        PORTC = ext_events & 0x3F;
    }
}
