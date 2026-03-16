#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "../assignment_common.h"
#include <avr/interrupt.h>
#include <util/delay.h>

/*
 * Suggested hardware validation points:
 * 1. Verify 5 V supply and local decoupling capacitors.
 * 2. Confirm reset pull-up and manual reset switch operation.
 * 3. Confirm clock source selection in fuse settings.
 * 4. Observe PB0 heartbeat LED and PB1 interrupt LED.
 */

volatile uint8_t interrupt_seen = 0;

ISR(INT0_vect)
{
    PORTB ^= (1 << PB1);
    interrupt_seen = 1;
}

int main(void)
{
    DDRB |= (1 << PB0) | (1 << PB1);
    PORTD |= (1 << PD2);

    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);
    sei();

    while (1) {
        PORTB ^= (1 << PB0);
        _delay_ms(250);

        if (interrupt_seen != 0U) {
            interrupt_seen = 0;
        }
    }
}
