#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"

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
    DDRD |= (1 << PD6);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01);

    while (1) {
        OCR0A = (uint8_t)(adc_read(0) >> 2);
    }
}
