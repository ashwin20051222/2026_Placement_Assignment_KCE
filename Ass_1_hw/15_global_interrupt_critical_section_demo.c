#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>
#include <stdint.h>

volatile uint16_t heartbeat = 0;

ISR(TIMER0_OVF_vect)
{
    ++heartbeat;
}

int main(void)
{
    volatile uint32_t delay_loop;
    uint16_t local_copy;

    DDRB |= (1 << PB0) | (1 << PB1);

    TCCR0A = 0x00;
    TCCR0B = (1 << CS02) | (1 << CS00);
    TIMSK0 = (1 << TOIE0);
    sei();

    while (1) {
        cli();
        local_copy = heartbeat;
        for (delay_loop = 0; delay_loop < 50000UL; ++delay_loop) {
        }
        sei();

        if ((local_copy & 0x20U) != 0U) {
            PORTB |= (1 << PB0);
        } else {
            PORTB &= (uint8_t)~(1 << PB0);
        }

        PORTB ^= (1 << PB1);
    }
}
