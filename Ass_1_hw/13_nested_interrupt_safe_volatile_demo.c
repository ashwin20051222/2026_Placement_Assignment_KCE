#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>
#include <stdint.h>

/*
 * volatile is required because overflow_count is modified inside an ISR and
 * read in main. Without volatile, the compiler may cache the value and miss
 * updates from the interrupt context.
 */
volatile uint16_t overflow_count = 0;

ISR(TIMER0_OVF_vect)
{
    ++overflow_count;
}

int main(void)
{
    uint16_t snapshot;

    DDRB = 0xFF;
    TCCR0A = 0x00;
    TCCR0B = (1 << CS02) | (1 << CS00);
    TIMSK0 = (1 << TOIE0);
    sei();

    while (1) {
        cli();
        snapshot = overflow_count;
        sei();

        PORTB = (uint8_t)(snapshot >> 2);
    }
}
