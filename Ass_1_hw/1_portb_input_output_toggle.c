#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"

int main(void)
{
    uint8_t previous_inputs = 0xF0;

    DDRB = 0x0F;
    PORTB = 0xF0;

    while (1) {
        uint8_t current_inputs = PINB & 0xF0;
        uint8_t changed = previous_inputs & (uint8_t)(~current_inputs);

        if (changed & (1 << PB4)) {
            PORTB ^= (1 << PB0);
        }
        if (changed & (1 << PB5)) {
            PORTB ^= (1 << PB1);
        }
        if (changed & (1 << PB6)) {
            PORTB ^= (1 << PB2);
        }
        if (changed & (1 << PB7)) {
            PORTB ^= (1 << PB3);
        }

        previous_inputs = current_inputs;
    }
}
