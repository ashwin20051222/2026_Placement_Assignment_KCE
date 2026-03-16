#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <util/delay.h>

int main(void)
{
    DDRB |= (1 << PB0);

    while (1) {
        PORTB ^= (1 << PB0);
        _delay_us(500);
    }
}
