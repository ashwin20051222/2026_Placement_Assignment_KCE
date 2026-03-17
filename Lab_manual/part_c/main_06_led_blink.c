#include "part_c_common.h"

int main() {
    Clock_EnableGpioC();
    Delay_InitSystick_1ms();
    Gpio_ConfigOutputPushPull(GPIOC, 13U);

    while(1) {
        Gpio_Set(GPIOC, 13U, 0);
        Delay_Ms(300U);
        Gpio_Set(GPIOC, 13U, 1);
        Delay_Ms(300U);
    }
}
