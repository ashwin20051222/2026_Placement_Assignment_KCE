#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <util/delay.h>

int main(void)
{
    DDRB |= (1 << PB0) | (1 << PB1);

    TCCR1A = (1 << COM1A0);
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 781;

    while (1) {
        PORTB ^= (1 << PB0);
        _delay_ms(50);
    }
}
