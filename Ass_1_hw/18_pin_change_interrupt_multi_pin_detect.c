#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>

volatile uint8_t previous_state = 0x07;

ISR(PCINT0_vect)
{
    uint8_t current_state = PINB & 0x07;
    uint8_t changed = current_state ^ previous_state;

    PORTD ^= changed;
    previous_state = current_state;
}

int main(void)
{
    DDRB &= (uint8_t)~0x07;
    PORTB |= 0x07;
    DDRD |= 0x07;

    PCICR = (1 << PCIE0);
    PCMSK0 = (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2);
    sei();

    while (1) {
    }
}
