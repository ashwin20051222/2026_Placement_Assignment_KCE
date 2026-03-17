#include "part_c_common.h"

void Motor_Set(int forward);

void Motor_Set(int forward) {
    if (forward) {
        Gpio_Set(GPIOA, 0U, 1);
        Gpio_Set(GPIOA, 1U, 0);
    } else {
        Gpio_Set(GPIOA, 0U, 0);
        Gpio_Set(GPIOA, 1U, 1);
    }
}

int main() {
    Clock_EnableGpioA();
    Delay_InitSystick_1ms();
    Gpio_ConfigOutputPushPull(GPIOA, 0U);
    Gpio_ConfigOutputPushPull(GPIOA, 1U);

    while(1) {
        Motor_Set(1);
        Delay_Ms(1000U);
        Motor_Set(0);
        Delay_Ms(1000U);
    }
}
