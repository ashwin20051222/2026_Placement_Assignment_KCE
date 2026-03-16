#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"

int main(void)
{
    DDRB |= (1 << PB0);

    TCCR1A = 0x00;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 15624;

    while (1) {
        TIFR1 = (1 << OCF1A);
        TCNT1 = 0;

        while ((TIFR1 & (1 << OCF1A)) == 0U) {
        }

        PORTB ^= (1 << PB0);
    }
}
