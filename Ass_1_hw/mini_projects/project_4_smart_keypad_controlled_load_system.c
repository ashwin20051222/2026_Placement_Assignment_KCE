#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "../assignment_common.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#define LCD_RS PD0
#define LCD_EN PD1
#define RELAY_PIN PB4

static const char keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

static const char password[5] = "1234";

volatile uint8_t one_second_tick = 0;
volatile uint8_t lockout_seconds = 0;

ISR(TIMER1_COMPA_vect)
{
    one_second_tick = 1;
    if (lockout_seconds > 0U) {
        --lockout_seconds;
    }
}

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

static void lcd_print_uint(uint8_t value)
{
    if (value >= 10U) {
        lcd_data((uint8_t)('0' + (value / 10U)));
    }
    lcd_data((uint8_t)('0' + (value % 10U)));
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

static uint8_t password_matches(const char *input)
{
    uint8_t index;

    for (index = 0; index < 4U; ++index) {
        if (input[index] != password[index]) {
            return 0;
        }
    }

    return 1;
}

static void show_ready_screen(uint8_t relay_on)
{
    lcd_command(0x01);
    lcd_print("Enter Password");
    lcd_command(0xC0);
    if (relay_on != 0U) {
        lcd_print("Load: ON");
    } else {
        lcd_print("Load: OFF");
    }
}

int main(void)
{
    char input_buffer[5] = {'\0', '\0', '\0', '\0', '\0'};
    uint8_t input_index = 0;
    uint8_t wrong_attempts = 0;
    uint8_t relay_on = 0;
    char key;

    DDRC |= 0x0F;
    DDRD &= (uint8_t)~0xF0;
    PORTD |= 0xF0;

    TCCR1A = 0x00;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 15624;
    TIMSK1 = (1 << OCIE1A);
    sei();

    lcd_init();
    show_ready_screen(relay_on);

    while (1) {
        if (one_second_tick != 0U) {
            one_second_tick = 0;

            if (lockout_seconds > 0U) {
                lcd_command(0x01);
                lcd_print("System Locked");
                lcd_command(0xC0);
                lcd_print("Wait ");
                lcd_print_uint(lockout_seconds);
                lcd_print(" sec");
            } else if (input_index == 0U) {
                show_ready_screen(relay_on);
            }
        }

        key = keypad_scan();
        if (key == '\0') {
            continue;
        }

        if (lockout_seconds > 0U) {
            while (keypad_scan() != '\0') {
            }
            _delay_ms(20);
            continue;
        }

        if (key == '*') {
            input_index = 0;
            input_buffer[0] = '\0';
            show_ready_screen(relay_on);
        } else if ((key >= '0' && key <= '9') && input_index < 4U) {
            input_buffer[input_index++] = key;
            input_buffer[input_index] = '\0';
            lcd_command(0xC0);
            lcd_print("Code: ");
            for (uint8_t i = 0; i < input_index; ++i) {
                lcd_data('*');
            }
        } else if (key == '#') {
            if (input_index == 4U && password_matches(input_buffer) != 0U) {
                relay_on ^= 1U;
                if (relay_on != 0U) {
                    PORTB |= (1 << RELAY_PIN);
                } else {
                    PORTB &= (uint8_t)~(1 << RELAY_PIN);
                }

                wrong_attempts = 0;
                lcd_command(0x01);
                lcd_print("Access Granted");
                lcd_command(0xC0);
                if (relay_on != 0U) {
                    lcd_print("Load Turned ON");
                } else {
                    lcd_print("Load Turned OFF");
                }
            } else {
                ++wrong_attempts;
                lcd_command(0x01);
                lcd_print("Wrong Password");
                lcd_command(0xC0);
                if (wrong_attempts >= 3U) {
                    lockout_seconds = 10;
                    wrong_attempts = 0;
                    lcd_print("Lockout 10 sec");
                } else {
                    lcd_print("Try Again");
                }
            }

            input_index = 0;
            input_buffer[0] = '\0';
        }

        while (keypad_scan() != '\0') {
        }
        _delay_ms(20);
    }
}
