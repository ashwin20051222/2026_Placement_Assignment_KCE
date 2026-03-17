#include "part_c_common.h"

int main() {
    Clock_EnableGpioC();
    Clock_EnableGpioA();
    Delay_InitSystick_1ms();

    Gpio_ConfigOutputPushPull(GPIOC, 13U);
    Gpio_ConfigInputPull(GPIOA, 0U, 1);

    int relay_on = 0;
    while(1) {
        int pressed = (Gpio_Get(GPIOA, 0U) == 0);
        if (pressed) {
            relay_on = !relay_on;
            Gpio_Set(GPIOC, 13U, relay_on ? 0 : 1);
            while (Gpio_Get(GPIOA, 0U) == 0) Delay_Ms(20U);
        }
        Delay_Ms(20U);
    }
}
