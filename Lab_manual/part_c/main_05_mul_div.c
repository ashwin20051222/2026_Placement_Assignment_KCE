#include "part_c_common.h"

void MulDiv(unsigned int a, unsigned int b, unsigned int *mulOut, unsigned int *divOut);

int main() {
    unsigned int mul = 0U;
    unsigned int div = 0U;
    MulDiv(42U, 6U, &mul, &div);

    Clock_EnableGpioC();
    Delay_InitSystick_1ms();
    Gpio_ConfigOutputPushPull(GPIOC, 13U);

    while(1) {
        int ok = (mul == 252U) && (div == 7U);
        Gpio_Set(GPIOC, 13U, ok ? 0 : 1);
        Delay_Ms(150U);
        Gpio_Set(GPIOC, 13U, 1);
        Delay_Ms(450U);
    }
}
