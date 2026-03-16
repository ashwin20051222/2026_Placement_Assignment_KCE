#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "../assignment_common.h"
#include <avr/interrupt.h>

typedef enum {
    STATE_NS_GREEN = 0,
    STATE_NS_YELLOW,
    STATE_EW_GREEN,
    STATE_EW_YELLOW,
    STATE_ALL_RED
} traffic_state_t;

volatile uint8_t second_ticks = 0;
volatile uint8_t emergency_request = 0;

ISR(TIMER1_COMPA_vect)
{
    ++second_ticks;
}

ISR(INT0_vect)
{
    emergency_request = 1;
}

static void apply_state(traffic_state_t state)
{
    PORTB &= (uint8_t)~((1 << PB0) | (1 << PB1) | (1 << PB2));
    PORTD &= (uint8_t)~((1 << PD5) | (1 << PD6) | (1 << PD7));

    switch (state) {
    case STATE_NS_GREEN:
        PORTB |= (1 << PB2);
        PORTD |= (1 << PD5);
        break;
    case STATE_NS_YELLOW:
        PORTB |= (1 << PB1);
        PORTD |= (1 << PD5);
        break;
    case STATE_EW_GREEN:
        PORTB |= (1 << PB0);
        PORTD |= (1 << PD7);
        break;
    case STATE_EW_YELLOW:
        PORTB |= (1 << PB0);
        PORTD |= (1 << PD6);
        break;
    case STATE_ALL_RED:
    default:
        PORTB |= (1 << PB0);
        PORTD |= (1 << PD5);
        break;
    }
}

int main(void)
{
    traffic_state_t state = STATE_NS_GREEN;
    uint8_t state_time = 0;

    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
    DDRD |= (1 << PD5) | (1 << PD6) | (1 << PD7);
    PORTD |= (1 << PD2);

    TCCR1A = 0x00;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 15624;
    TIMSK1 = (1 << OCIE1A);

    EICRA = (1 << ISC01);
    EIMSK = (1 << INT0);
    sei();

    apply_state(state);

    while (1) {
        if (second_ticks == 0U) {
            continue;
        }

        --second_ticks;

        if (emergency_request != 0U) {
            state = STATE_ALL_RED;
            state_time = 0;
            emergency_request = 0;
            apply_state(state);
            continue;
        }

        ++state_time;

        switch (state) {
        case STATE_NS_GREEN:
            if (state_time >= 5U) {
                state = STATE_NS_YELLOW;
                state_time = 0;
                apply_state(state);
            }
            break;
        case STATE_NS_YELLOW:
            if (state_time >= 2U) {
                state = STATE_EW_GREEN;
                state_time = 0;
                apply_state(state);
            }
            break;
        case STATE_EW_GREEN:
            if (state_time >= 5U) {
                state = STATE_EW_YELLOW;
                state_time = 0;
                apply_state(state);
            }
            break;
        case STATE_EW_YELLOW:
            if (state_time >= 2U) {
                state = STATE_NS_GREEN;
                state_time = 0;
                apply_state(state);
            }
            break;
        case STATE_ALL_RED:
        default:
            if (state_time >= 5U) {
                state = STATE_NS_GREEN;
                state_time = 0;
                apply_state(state);
            }
            break;
        }
    }
}
