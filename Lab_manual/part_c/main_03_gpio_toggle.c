#include "part_c_common.h"

void Gpio_Toggle_BSRR(unsigned int *odr, unsigned int *bsrr, unsigned int pinMask);

int main() {
    Clock_EnableGpioC();
    Delay_InitSystick_1ms();
    Gpio_ConfigOutputPushPull(GPIOC, 13U);

    while(1) {
        Gpio_Toggle_BSRR((unsigned int *)&GPIOC->ODR, (unsigned int *)&GPIOC->BSRR, (1U << 13U));
        Delay_Ms(250U);
    }
}
