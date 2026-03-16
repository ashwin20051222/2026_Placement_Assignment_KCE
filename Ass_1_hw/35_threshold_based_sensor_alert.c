#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <stdint.h>

#define SENSOR_THRESHOLD 700U

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
    uint16_t sensor_value;

    DDRB |= (1 << PB0) | (1 << PB1);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    while (1) {
        sensor_value = adc_read(0);

        if (sensor_value >= SENSOR_THRESHOLD) {
            PORTB |= (1 << PB0);
        } else {
            PORTB &= (uint8_t)~(1 << PB0);
        }

        if ((sensor_value & 0x20U) != 0U) {
            PORTB |= (1 << PB1);
        } else {
            PORTB &= (uint8_t)~(1 << PB1);
        }
    }
}
