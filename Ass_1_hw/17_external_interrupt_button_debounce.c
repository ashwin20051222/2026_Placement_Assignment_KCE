#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>
#include <stdint.h>

volatile uint32_t ms_ticks = 0;
volatile uint32_t last_press_ms = 0;

ISR(TIMER0_OVF_vect)
{
    ++ms_ticks;
}

ISR(INT0_vect)
{
    uint32_t now = ms_ticks;

    if ((now - last_press_ms) >= 30UL) {
        PORTB ^= (1 << PB0);
        last_press_ms = now;
    }
}

int main(void)
{
    DDRB |= (1 << PB0);
    PORTD |= (1 << PD2);

    TCCR0A = 0x00;
    TCCR0B = (1 << CS01) | (1 << CS00);
    TIMSK0 = (1 << TOIE0);

    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);
    sei();

    while (1) {
    }
}
