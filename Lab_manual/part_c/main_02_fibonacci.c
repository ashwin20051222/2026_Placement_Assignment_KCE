#include "part_c_common.h"

void Fib10_Fill(unsigned int *out10);

int main() {
    unsigned int fib[10];
    unsigned int sum = 0U;

    Fib10_Fill(fib);
    for (unsigned int i = 0U; i < 10U; i++) sum += fib[i];

    Clock_EnableGpioC();
    Delay_InitSystick_1ms();
    Gpio_ConfigOutputPushPull(GPIOC, 13U);

    while(1) {

        unsigned int blinks = sum % 10U;
        if (blinks == 0U) blinks = 1U;
        for (unsigned int i = 0U; i < blinks; i++) {
            Gpio_Set(GPIOC, 13U, 0);
            Delay_Ms(120U);
            Gpio_Set(GPIOC, 13U, 1);
            Delay_Ms(120U);
        }
        Delay_Ms(600U);
    }
}
