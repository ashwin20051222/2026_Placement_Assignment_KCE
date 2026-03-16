#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <util/delay.h>

static const uint8_t segment_map[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

int main(void)
{
    uint8_t digit;

    DDRD = 0x7F;

    while (1) {
        for (digit = 0; digit < 10; ++digit) {
            PORTD = segment_map[digit];
            _delay_ms(500);
        }
    }
}
