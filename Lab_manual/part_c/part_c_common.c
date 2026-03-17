#include "part_c_common.h"

volatile unsigned int g_ms_ticks;

void SysTick_Handler(void) {
    g_ms_ticks++;
}

void Delay_InitSystick_1ms(void) {
    g_ms_ticks = 0;
    SysTick->LOAD = (SystemCoreClock / 1000U) - 1U;
    SysTick->VAL = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void Delay_Ms(unsigned int ms) {
    unsigned int start = g_ms_ticks;
    while ((g_ms_ticks - start) < ms) {
        __NOP();
    }
}

void Clock_EnableAfio(void) {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
}

void Clock_EnableGpioA(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
}

void Clock_EnableGpioB(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
}

void Clock_EnableGpioC(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
}

void Gpio_ConfigOutputPushPull(GPIO_TypeDef *port, unsigned int pin) {
    unsigned int shift = (pin % 8U) * 4U;
    volatile unsigned int *cr = (pin < 8U) ? &port->CRL : &port->CRH;
    unsigned int v = *cr;
    v &= ~(0xFU << shift);
    v |= (0x2U << shift);
    *cr = v;
}

void Gpio_ConfigInputPull(GPIO_TypeDef *port, unsigned int pin, int pull_up) {
    unsigned int shift = (pin % 8U) * 4U;
    volatile unsigned int *cr = (pin < 8U) ? &port->CRL : &port->CRH;
    unsigned int v = *cr;
    v &= ~(0xFU << shift);
    v |= (0x8U << shift);
    *cr = v;
    if (pull_up) port->BSRR = (1U << pin);
    else port->BRR = (1U << pin);
}

void Gpio_Set(GPIO_TypeDef *port, unsigned int pin, int high) {
    if (high) port->BSRR = (1U << pin);
    else port->BRR = (1U << pin);
}

int Gpio_Get(GPIO_TypeDef *port, unsigned int pin) {
    return (port->IDR & (1U << pin)) ? 1 : 0;
}

void Usart1_Init_9600_8N1(void) {
    Clock_EnableAfio();
    Clock_EnableGpioA();
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    {
        unsigned int v;
        v = GPIOA->CRH;
        v &= ~(0xFU << ((9U - 8U) * 4U));
        v |= (0xBU << ((9U - 8U) * 4U));
        v &= ~(0xFU << ((10U - 8U) * 4U));
        v |= (0x4U << ((10U - 8U) * 4U));
        GPIOA->CRH = v;
    }

    USART1->BRR = SystemCoreClock / 9600U;
    USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void Usart1_WriteChar(char c) {
    while (!(USART1->SR & USART_SR_TXE)) {}
    USART1->DR = (unsigned int)c;
}

char Usart1_ReadCharBlocking(void) {
    while (!(USART1->SR & USART_SR_RXNE)) {}
    return (char)(USART1->DR & 0xFFU);
}

void Usart1_WriteString(const char *s) {
    while (*s) {
        Usart1_WriteChar(*s++);
    }
}
