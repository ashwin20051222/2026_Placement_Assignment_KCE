#include "part_c_common.h"

int main() {
    Delay_InitSystick_1ms();
    Usart1_Init_9600_8N1();
    Usart1_WriteString("USART1 echo ready\r\n");

    while(1) {
        char c = Usart1_ReadCharBlocking();
        Usart1_WriteChar(c);
    }
}
