#include "part_c_common.h"

void Add32_StoreResult(unsigned int a, unsigned int b, unsigned int *out);

int main() {
    unsigned int a = 100U;
    unsigned int b = 23U;
    unsigned int result = 0U;

    Add32_StoreResult(a, b, &result);

    Clock_EnableGpioC();
    Delay_InitSystick_1ms();
    Gpio_ConfigOutputPushPull(GPIOC, 13U);

    while(1) {

        int ok = (result == (a + b)) ? 1 : 0;
        Gpio_Set(GPIOC, 13U, ok ? 0 : 1);
        Delay_Ms(300U);
        Gpio_Set(GPIOC, 13U, 1);
        Delay_Ms(300U);
    }
}
