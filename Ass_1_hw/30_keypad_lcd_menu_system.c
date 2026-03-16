#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "assignment_common.h"
#include <util/delay.h>
#include <stdint.h>

#define LCD_RS PD0
#define LCD_EN PD1

static const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

static void lcd_pulse_enable(void)
{
    PORTD |= (1 << LCD_EN);
    _delay_us(1);
    PORTD &= (uint8_t)~(1 << LCD_EN);
    _delay_us(100);
}

static void lcd_send_nibble(uint8_t nibble)
{
    PORTB = (PORTB & 0xF0) | (nibble & 0x0F);
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

static void lcd_print_uint(uint16_t value)
{
    char buffer[6];
    uint8_t index = 0;

    if (value == 0U) {
        lcd_data('0');
        return;
    }

    while (value > 0U) {
        buffer[index++] = (char)('0' + (value % 10U));
        value /= 10U;
    }

    while (index > 0U) {
        lcd_data((uint8_t)buffer[--index]);
    }
}

static void lcd_init(void)
{
    DDRB |= 0x1F;
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

static uint16_t adc_read(uint8_t channel)
{
    ADMUX = (1 << REFS0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);

    while ((ADCSRA & (1 << ADSC)) != 0U) {
    }

    return ADC;
}

static void show_menu(uint8_t item)
{
    lcd_command(0x01);

    if (item == 0U) {
        lcd_print("1. LED Control");
        lcd_command(0xC0);
        lcd_print("# Toggle");
    } else if (item == 1U) {
        lcd_print("2. ADC View");
        lcd_command(0xC0);
        lcd_print("# Read ADC5");
    } else {
        lcd_print("3. About");
        lcd_command(0xC0);
        lcd_print("# Details");
    }
}

int main(void)
{
    uint8_t menu_item = 0;
    char key;

    DDRC |= 0x0F;
    DDRD &= (uint8_t)~0xF0;
    PORTD |= 0xF0;
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    lcd_init();
    show_menu(menu_item);

    while (1) {
        key = keypad_scan();

        if (key == '\0') {
            continue;
        }

        if (key == '2') {
            menu_item = (uint8_t)((menu_item + 1U) % 3U);
            show_menu(menu_item);
        } else if (key == '8') {
            menu_item = (uint8_t)((menu_item + 2U) % 3U);
            show_menu(menu_item);
        } else if (key == '#') {
            lcd_command(0x01);

            if (menu_item == 0U) {
                PORTB ^= (1 << PB4);
                lcd_print("LED Toggled");
            } else if (menu_item == 1U) {
                lcd_print("ADC5 Value:");
                lcd_command(0xC0);
                lcd_print_uint(adc_read(5));
            } else {
                lcd_print("Bare-Metal Menu");
                lcd_command(0xC0);
                lcd_print("Keypad + LCD");
            }
        } else if (key == '*') {
            show_menu(menu_item);
        }

        while (keypad_scan() != '\0') {
        }
        _delay_ms(20);
    }
}
