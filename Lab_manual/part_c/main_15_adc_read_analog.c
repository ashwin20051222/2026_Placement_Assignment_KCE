#include "part_c_common.h"

void Adc1_Init_Channel0(void);
unsigned int Adc1_Read_Channel0(void);

void Adc1_Init_Channel0(void) {
    Clock_EnableGpioA();
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    GPIOA->CRL &= ~(0xFU << (0U * 4U));

    ADC1->CR2 = ADC_CR2_ADON;
    Delay_Ms(1U);
    ADC1->SMPR2 |= (7U << 0U);
    ADC1->SQR3 = 0U;
}

unsigned int Adc1_Read_Channel0(void) {
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC)) {}
    return ADC1->DR;
}

int main() {
    Delay_InitSystick_1ms();
    Usart1_Init_9600_8N1();
    Adc1_Init_Channel0();

    while(1) {
        unsigned int v = Adc1_Read_Channel0();
        char buf[64];
        (void)sprintf(buf, "ADC0=%u\r\n", v);
        Usart1_WriteString(buf);
        Delay_Ms(500U);
    }
}
