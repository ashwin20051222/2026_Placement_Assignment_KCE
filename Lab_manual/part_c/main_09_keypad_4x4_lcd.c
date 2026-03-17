#include "part_c_common.h"

char Keypad_Scan(void);

char Keypad_Scan(void) {
    char map[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    for (unsigned int r = 0U; r < 4U; r++) {
        for (unsigned int i = 0U; i < 4U; i++) Gpio_Set(GPIOA, i, 1);
        Gpio_Set(GPIOA, r, 0);
        Delay_Ms(1U);
        for (unsigned int c = 0U; c < 4U; c++) {
            if (Gpio_Get(GPIOA, 4U + c) == 0) return map[r][c];
        }
    }
    return 0;
}

int main() {
    Clock_EnableGpioA();
    Delay_InitSystick_1ms();
    Usart1_Init_9600_8N1();

    for (unsigned int i = 0U; i < 4U; i++) Gpio_ConfigOutputPushPull(GPIOA, i);
    for (unsigned int i = 4U; i < 8U; i++) Gpio_ConfigInputPull(GPIOA, i, 1);

    Usart1_WriteString("Keypad scan (press keys)\r\n");
    while(1) {
        char k = Keypad_Scan();
        if (k) {
            Usart1_WriteChar(k);
            Usart1_WriteString("\r\n");
            while (Keypad_Scan()) Delay_Ms(20U);
        }
        Delay_Ms(50U);
    }
}
