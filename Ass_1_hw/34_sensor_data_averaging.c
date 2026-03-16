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
    uint16_t samples[16] = {0};
    uint32_t running_sum = 0;
    uint8_t index = 0;
    uint16_t new_sample;
    uint16_t average;

    DDRB = 0xFF;
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    while (1) {
        new_sample = adc_read(0);
        running_sum -= samples[index];
        samples[index] = new_sample;
        running_sum += new_sample;
        index = (uint8_t)((index + 1U) & 0x0FU);

        average = (uint16_t)(running_sum / 16UL);
        PORTB = (uint8_t)(average >> 2);
    }
}
