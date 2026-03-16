#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <stdint.h>
#include <util/delay.h>

static uint16_t read_adc_with_reference(uint8_t reference_bits)
{
    ADMUX = reference_bits | 0;
    _delay_ms(2);

    ADCSRA |= (1 << ADSC);
    while ((ADCSRA & (1 << ADSC)) != 0U) {
    }

    ADCSRA |= (1 << ADSC);
    while ((ADCSRA & (1 << ADSC)) != 0U) {
    }

    return ADC;
}

int main(void)
{
    uint16_t avcc_value;
    uint16_t internal_value;
    uint16_t difference;

    DDRB = 0xFF;
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    while (1) {
        avcc_value = read_adc_with_reference(1 << REFS0);
        internal_value = read_adc_with_reference((1 << REFS1) | (1 << REFS0));

        if (avcc_value > internal_value) {
            difference = avcc_value - internal_value;
        } else {
            difference = internal_value - avcc_value;
        }

        PORTB = (uint8_t)(difference >> 2);
    }
}
