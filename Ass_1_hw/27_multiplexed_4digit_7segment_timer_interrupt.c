#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>
#include <util/delay.h>

static const uint8_t segment_map[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

volatile uint16_t display_value = 0;
volatile uint8_t current_digit = 0;

ISR(TIMER0_COMPA_vect)
{
    uint16_t value = display_value;
    uint8_t digit_value;

    PORTB &= (uint8_t)~0x0F;

    if (current_digit == 0U) {
        digit_value = value % 10U;
    } else if (current_digit == 1U) {
        digit_value = (value / 10U) % 10U;
    } else if (current_digit == 2U) {
        digit_value = (value / 100U) % 10U;
    } else {
        digit_value = (value / 1000U) % 10U;
    }

    PORTD = segment_map[digit_value];
    PORTB |= (1 << current_digit);
    current_digit = (uint8_t)((current_digit + 1U) & 0x03U);
}

int main(void)
{
    DDRD = 0x7F;
    DDRB |= 0x0F;

    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);
    OCR0A = 249;
    TIMSK0 = (1 << OCIE0A);
    sei();

    while (1) {
        _delay_ms(100);
        display_value = (uint16_t)((display_value + 1U) % 10000U);
    }
}
