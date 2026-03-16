#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <util/delay.h>

static void update_duty_cycle(uint8_t *duty, uint8_t pressed, uint8_t step)
{
    if (pressed == 0U && *duty <= (uint8_t)(255U - step)) {
        *duty += step;
        _delay_ms(150);
    }
}

int main(void)
{
    uint8_t duty = 128;

    DDRD |= (1 << PD6);
    DDRB &= (uint8_t)~((1 << PB0) | (1 << PB1));
    PORTB |= (1 << PB0) | (1 << PB1);

    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01);
    OCR0A = duty;

    while (1) {
        update_duty_cycle(&duty, PINB & (1 << PB0), 16);

        if ((PINB & (1 << PB1)) == 0U && duty >= 16U) {
            duty -= 16;
            _delay_ms(150);
        }

        OCR0A = duty;
    }
}
