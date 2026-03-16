#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"

static void delay_10ms_timer0(void)
{
    TCNT0 = 100;
    TIFR0 = (1 << TOV0);

    while ((TIFR0 & (1 << TOV0)) == 0U) {
    }
}

int main(void)
{
    DDRB |= (1 << PB0);
    TCCR0A = 0x00;
    TCCR0B = (1 << CS02) | (1 << CS00);

    while (1) {
        delay_10ms_timer0();
        PORTB ^= (1 << PB0);
    }
}
