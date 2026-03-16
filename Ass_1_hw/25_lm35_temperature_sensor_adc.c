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
    uint16_t temperature_c;

    DDRB = 0xFF;
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    while (1) {
        adc_value = adc_read(0);
        temperature_c = (adc_value * 500UL) / 1024UL;
        PORTB = (uint8_t)temperature_c;
    }
}
