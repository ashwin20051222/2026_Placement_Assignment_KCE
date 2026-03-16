#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <avr/interrupt.h>
#include <stdint.h>

volatile uint16_t samples[64];
volatile uint8_t sample_index = 0;
volatile uint8_t buffer_full = 0;

ISR(ADC_vect)
{
    if (buffer_full == 0U) {
        samples[sample_index] = ADC;
        ++sample_index;

        if (sample_index >= 64U) {
            sample_index = 0;
            buffer_full = 1;
            ADCSRA &= (uint8_t)~((1 << ADATE) | (1 << ADIE));
        }
    }
}

int main(void)
{
    DDRB = 0xFF;
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADATE) | (1 << ADIE) |
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRB = 0x00;
    sei();
    ADCSRA |= (1 << ADSC);

    while (1) {
        PORTB = (uint8_t)(samples[(sample_index == 0U) ? 0U : (sample_index - 1U)] >> 2);
    }
}
