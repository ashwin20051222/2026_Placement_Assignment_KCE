#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <stdint.h>

static uint16_t adc_read(uint8_t channel)
{
    ADMUX = (1 << REFS0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);

    while ((ADCSRA & (1 << ADSC)) != 0U) {
    }

    return ADC;
}

int main(void)
{
    uint16_t adc_value;
    uint8_t level;

    DDRB = 0xFF;
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    while (1) {
        adc_value = adc_read(0);
        level = (uint8_t)((adc_value * 8UL) / 1024UL);

        if (level == 0U) {
            PORTB = 0x00;
        } else if (level >= 8U) {
            PORTB = 0xFF;
        } else {
            PORTB = (uint8_t)((1U << level) - 1U);
        }
    }
}
