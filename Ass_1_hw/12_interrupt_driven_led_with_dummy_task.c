#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>
#include <stdint.h>

volatile uint8_t tick_100ms = 0;
volatile uint32_t dummy_accumulator = 0;

ISR(TIMER1_COMPA_vect)
{
    ++tick_100ms;
    if (tick_100ms >= 5U) {
        PORTB ^= (1 << PB0);
        tick_100ms = 0;
    }
}

int main(void)
{
    DDRB |= (1 << PB0);

    TCCR1A = 0x00;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 1562;
    TIMSK1 = (1 << OCIE1A);
    sei();

    while (1) {
        dummy_accumulator += 37U;
        dummy_accumulator ^= 0x55AA55AAUL;
    }
}
