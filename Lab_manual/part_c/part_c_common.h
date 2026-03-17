#pragma once

#include "stm32f10x.h"

void Clock_EnableGpioA(void);
void Clock_EnableGpioB(void);
void Clock_EnableGpioC(void);
void Clock_EnableAfio(void);
void Delay_InitSystick_1ms(void);
void Delay_Ms(unsigned int ms);

void Gpio_ConfigOutputPushPull(GPIO_TypeDef *port, unsigned int pin);
void Gpio_ConfigInputPull(GPIO_TypeDef *port, unsigned int pin, int pull_up);
void Gpio_Set(GPIO_TypeDef *port, unsigned int pin, int high);
int  Gpio_Get(GPIO_TypeDef *port, unsigned int pin);

void Usart1_Init_9600_8N1(void);
void Usart1_WriteChar(char c);
char Usart1_ReadCharBlocking(void);
void Usart1_WriteString(const char *s);
