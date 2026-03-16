#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <util/delay.h>

static inline void od_drive_low(void)
{
    PORTB &= ~(1 << PB0);
    DDRB |= (1 << PB0);
}

static inline void od_release_line(void)
{
    PORTB &= ~(1 << PB0);
    DDRB &= ~(1 << PB0);
}

int main(void)
{
    od_release_line();

    while (1) {
        od_drive_low();
        _delay_ms(500);

        od_release_line();
        _delay_ms(500);
    }
}
