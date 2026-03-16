#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "../assignment_common.h"
#include <util/delay.h>
#include <stdint.h>

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

static uint16_t adc_read(uint8_t channel)
{
    ADMUX = (1 << REFS0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);

    while ((ADCSRA & (1 << ADSC)) != 0U) {
    }

    return ADC;
}

static void lcd_print_voltage(uint16_t millivolts)
{
    lcd_data((uint8_t)('0' + (millivolts / 1000U)));
    lcd_data('.');
    lcd_data((uint8_t)('0' + ((millivolts / 100U) % 10U)));
    lcd_data((uint8_t)('0' + ((millivolts / 10U) % 10U)));
    lcd_data((uint8_t)('0' + (millivolts % 10U)));
    lcd_data('V');
}

int main(void)
{
    uint16_t adc_value;
    uint16_t millivolts;

    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    lcd_init();

    while (1) {
        adc_value = adc_read(0);
        millivolts = (uint16_t)((adc_value * 5000UL) / 1023UL);

        lcd_command(0x01);
        lcd_print("Digital Voltmeter");
        lcd_command(0xC0);
        lcd_print("Vin = ");
        lcd_print_voltage(millivolts);
        _delay_ms(250);
    }
}
