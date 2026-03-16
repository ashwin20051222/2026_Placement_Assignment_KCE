#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <util/delay.h>

int main(void)
{
    uint8_t value = 0;

    DDRD |= (1 << PD6);

    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS00);

    while (1) {
        OCR0A = value++;
        _delay_ms(20);
    }
}
