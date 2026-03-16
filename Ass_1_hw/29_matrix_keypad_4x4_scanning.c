#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <util/delay.h>

static const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

static char keypad_scan(void)
{
    uint8_t row;
    uint8_t column;

    for (row = 0; row < 4; ++row) {
        PORTC |= 0x0F;
        PORTC &= (uint8_t)~(1 << row);
        _delay_us(5);

        for (column = 0; column < 4; ++column) {
            if ((PIND & (1 << (PD4 + column))) == 0U) {
                return keymap[row][column];
            }
        }
    }

    return '\0';
}

int main(void)
{
    char key;

    DDRC |= 0x0F;
    DDRD &= (uint8_t)~0xF0;
    PORTD |= 0xF0;
    DDRB = 0xFF;

    while (1) {
        key = keypad_scan();

        if (key != '\0') {
            PORTB = (uint8_t)key;
            while (keypad_scan() != '\0') {
            }
            _delay_ms(20);
        }
    }
}
