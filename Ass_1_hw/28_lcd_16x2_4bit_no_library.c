#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <util/delay.h>

#define LCD_RS PD0
#define LCD_EN PD1

static void lcd_pulse_enable(void)
{
    PORTD |= (1 << LCD_EN);
    _delay_us(1);
    PORTD &= (uint8_t)~(1 << LCD_EN);
    _delay_us(100);
}

static void lcd_send_nibble(uint8_t nibble)
{
    PORTB &= 0xF0;
    PORTB |= (nibble & 0x0F);
    lcd_pulse_enable();
}

static void lcd_send_byte(uint8_t value, uint8_t is_data)
{
    if (is_data != 0U) {
        PORTD |= (1 << LCD_RS);
    } else {
        PORTD &= (uint8_t)~(1 << LCD_RS);
    }

    lcd_send_nibble(value >> 4);
    lcd_send_nibble(value);
}

static void lcd_command(uint8_t command)
{
    lcd_send_byte(command, 0);
    _delay_ms(2);
}

static void lcd_data(uint8_t data)
{
    lcd_send_byte(data, 1);
}

static void lcd_print(const char *text)
{
    while (*text != '\0') {
        lcd_data((uint8_t)*text++);
    }
}

static void lcd_init(void)
{
    DDRB |= 0x0F;
    DDRD |= (1 << LCD_RS) | (1 << LCD_EN);
    _delay_ms(20);

    lcd_send_nibble(0x03);
    _delay_ms(5);
    lcd_send_nibble(0x03);
    _delay_us(200);
    lcd_send_nibble(0x03);
    lcd_send_nibble(0x02);

    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
}

int main(void)
{
    lcd_init();
    lcd_print("Bare Metal AVR");
    lcd_command(0xC0);
    lcd_print("LCD 4-bit Mode");

    while (1) {
    }
}
