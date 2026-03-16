#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

volatile uint16_t echo_start = 0;
volatile uint16_t echo_ticks = 0;
volatile uint8_t capture_ready = 0;
volatile uint8_t waiting_for_falling_edge = 0;

ISR(TIMER1_CAPT_vect)
{
    if (waiting_for_falling_edge == 0U) {
        echo_start = ICR1;
        TCCR1B &= (uint8_t)~(1 << ICES1);
        waiting_for_falling_edge = 1;
    } else {
        echo_ticks = (uint16_t)(ICR1 - echo_start);
        TCCR1B |= (1 << ICES1);
        waiting_for_falling_edge = 0;
        capture_ready = 1;
    }
}

static void trigger_sensor(void)
{
    PORTB &= (uint8_t)~(1 << PB1);
    _delay_us(2);
    PORTB |= (1 << PB1);
    _delay_us(10);
    PORTB &= (uint8_t)~(1 << PB1);
}

int main(void)
{
    uint16_t distance_cm;

    DDRB |= (1 << PB1);
    DDRD = 0xFF;

    TCCR1A = 0x00;
    TCCR1B = (1 << ICES1) | (1 << CS11);
    TIMSK1 = (1 << ICIE1);
    sei();

    while (1) {
        capture_ready = 0;
        trigger_sensor();

        while (capture_ready == 0U) {
        }

        distance_cm = echo_ticks / 116U;
        PORTD = (uint8_t)distance_cm;
        _delay_ms(60);
    }
}
