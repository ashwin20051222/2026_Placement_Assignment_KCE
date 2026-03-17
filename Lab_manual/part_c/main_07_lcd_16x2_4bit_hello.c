#include "part_c_common.h"

void Lcd_PulseEnable(void);
void Lcd_Write4(unsigned int nibble);
void Lcd_WriteCmd(unsigned int cmd);
void Lcd_WriteData(unsigned int data);
void Lcd_Init4bit(void);
void Lcd_SetCursor(unsigned int row, unsigned int col);
void Lcd_Print(const char *s);

void Lcd_PulseEnable(void) {
    Gpio_Set(GPIOB, 1U, 1);
    Delay_Ms(1U);
    Gpio_Set(GPIOB, 1U, 0);
    Delay_Ms(1U);
}

void Lcd_Write4(unsigned int nibble) {
    Gpio_Set(GPIOB, 2U, (nibble >> 0) & 1U);
    Gpio_Set(GPIOB, 10U, (nibble >> 1) & 1U);
    Gpio_Set(GPIOB, 11U, (nibble >> 2) & 1U);
    Gpio_Set(GPIOB, 12U, (nibble >> 3) & 1U);
    Lcd_PulseEnable();
}

void Lcd_WriteCmd(unsigned int cmd) {
    Gpio_Set(GPIOB, 0U, 0);
    Lcd_Write4(cmd >> 4);
    Lcd_Write4(cmd & 0x0FU);
    Delay_Ms(2U);
}

void Lcd_WriteData(unsigned int data) {
    Gpio_Set(GPIOB, 0U, 1);
    Lcd_Write4(data >> 4);
    Lcd_Write4(data & 0x0FU);
    Delay_Ms(2U);
}

void Lcd_Init4bit(void) {
    Clock_EnableGpioB();
    Delay_InitSystick_1ms();
    Gpio_ConfigOutputPushPull(GPIOB, 0U);
    Gpio_ConfigOutputPushPull(GPIOB, 1U);
    Gpio_ConfigOutputPushPull(GPIOB, 2U);
    Gpio_ConfigOutputPushPull(GPIOB, 10U);
    Gpio_ConfigOutputPushPull(GPIOB, 11U);
    Gpio_ConfigOutputPushPull(GPIOB, 12U);

    Delay_Ms(40U);
    Gpio_Set(GPIOB, 0U, 0);
    Gpio_Set(GPIOB, 1U, 0);

    Lcd_Write4(0x3U);
    Delay_Ms(5U);
    Lcd_Write4(0x3U);
    Delay_Ms(5U);
    Lcd_Write4(0x3U);
    Delay_Ms(1U);
    Lcd_Write4(0x2U);

    Lcd_WriteCmd(0x28U);
    Lcd_WriteCmd(0x0CU);
    Lcd_WriteCmd(0x06U);
    Lcd_WriteCmd(0x01U);
    Delay_Ms(5U);
}

void Lcd_SetCursor(unsigned int row, unsigned int col) {
    unsigned int addr = (row == 0U) ? 0x00U : 0x40U;
    addr += col;
    Lcd_WriteCmd(0x80U | addr);
}

void Lcd_Print(const char *s) {
    while (*s) Lcd_WriteData((unsigned int)*s++);
}

int main() {
    Lcd_Init4bit();
    Lcd_SetCursor(0U, 0U);
    Lcd_Print("HELLO WORLD");
    while(1) {}
}
