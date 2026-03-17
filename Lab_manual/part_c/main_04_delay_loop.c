#include "part_c_common.h"

void Delay_Loop(unsigned int count);

int main() {
    Clock_EnableGpioC();
    Gpio_ConfigOutputPushPull(GPIOC, 13U);

    while(1) {
        Gpio_Set(GPIOC, 13U, 0);
        Delay_Loop(800000U);
        Gpio_Set(GPIOC, 13U, 1);
        Delay_Loop(800000U);
    }
}
