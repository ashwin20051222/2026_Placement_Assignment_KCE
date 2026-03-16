#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"

#define SET_BIT(reg, bit) ((reg) |= (1U << (bit)))
#define CLEAR_BIT(reg, bit) ((reg) &= (uint8_t)~(1U << (bit)))
#define TOGGLE_BIT(reg, bit) ((reg) ^= (1U << (bit)))
#define READ_BIT(reg, bit) (((reg) >> (bit)) & 0x01U)

int main(void)
{
    SET_BIT(DDRB, PB0);
    CLEAR_BIT(DDRD, PD2);
    SET_BIT(PORTD, PD2);

    while (1) {
        if (READ_BIT(PIND, PD2) == 0U) {
            TOGGLE_BIT(PORTB, PB0);
            while (READ_BIT(PIND, PD2) == 0U) {
            }
        }
    }
}
