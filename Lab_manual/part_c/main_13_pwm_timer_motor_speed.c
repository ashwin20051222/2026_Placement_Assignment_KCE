#include "part_c_common.h"

void Pwm_Tim2Ch1_Init(void);
void Pwm_Tim2Ch1_SetDutyPermille(unsigned int permille);

void Pwm_Tim2Ch1_Init(void) {
    Clock_EnableGpioA();
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    {
        unsigned int v = GPIOA->CRL;
        v &= ~(0xFU << (0U * 4U));
        v |= (0xBU << (0U * 4U));
        GPIOA->CRL = v;
    }

    TIM2->PSC = 72U - 1U;
    TIM2->ARR = 1000U - 1U;
    TIM2->CCR1 = 0U;
    TIM2->CCMR1 = (6U << 4U) | TIM_CCMR1_OC1PE;
    TIM2->CCER = TIM_CCER_CC1E;
    TIM2->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
}

void Pwm_Tim2Ch1_SetDutyPermille(unsigned int permille) {
    if (permille > 1000U) permille = 1000U;
    TIM2->CCR1 = permille;
}

int main() {
    Delay_InitSystick_1ms();
    Pwm_Tim2Ch1_Init();

    while(1) {
        for (unsigned int d = 0U; d <= 1000U; d += 50U) {
            Pwm_Tim2Ch1_SetDutyPermille(d);
            Delay_Ms(120U);
        }
        for (unsigned int d = 1000U; d > 0U; d -= 50U) {
            Pwm_Tim2Ch1_SetDutyPermille(d);
            Delay_Ms(120U);
        }
    }
}
