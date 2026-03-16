#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <stdint.h>

static void wait_1ms_timer0(void)
{
    TIFR0 = (1 << OCF0A);
    TCNT0 = 0;

    while ((TIFR0 & (1 << OCF0A)) == 0U) {
    }
}

int main(void)
{
    uint16_t pulse_count;
    uint16_t gate_ms;

    DDRB = 0xFF;

    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);
    OCR0A = 249;

    TCCR1A = 0x00;
    TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10);

    while (1) {
        TCNT1 = 0;

        for (gate_ms = 0; gate_ms < 1000; ++gate_ms) {
            wait_1ms_timer0();
        }

        pulse_count = TCNT1;
        PORTB = (uint8_t)pulse_count;
    }
}
