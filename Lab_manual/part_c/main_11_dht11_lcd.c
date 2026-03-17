#include "part_c_common.h"

void Dht11_SetOutput(void);
void Dht11_SetInputPullup(void);
int  Dht11_ReadByte(unsigned int *out);
int  Dht11_Read(int *temp_c, int *humidity);

void Dht11_SetOutput(void) {
    Gpio_ConfigOutputPushPull(GPIOA, 1U);
}

void Dht11_SetInputPullup(void) {
    Gpio_ConfigInputPull(GPIOA, 1U, 1);
}

int Dht11_ReadByte(unsigned int *out) {
    unsigned int v = 0U;
    for (unsigned int i = 0U; i < 8U; i++) {
        unsigned int t = 0U;
        while (Gpio_Get(GPIOA, 1U) == 0) { if (++t > 80000U) return 0; }
        t = 0U;
        while (Gpio_Get(GPIOA, 1U) == 1) { if (++t > 80000U) break; }
        v <<= 1;
        if (t > 2000U) v |= 1U;
    }
    *out = v;
    return 1;
}

int Dht11_Read(int *temp_c, int *humidity) {
    unsigned int hI, hD, tI, tD, cs;

    Dht11_SetOutput();
    Gpio_Set(GPIOA, 1U, 0);
    Delay_Ms(20U);
    Gpio_Set(GPIOA, 1U, 1);

    Dht11_SetInputPullup();
    Delay_Ms(1U);

    unsigned int t = 0U;
    while (Gpio_Get(GPIOA, 1U) == 1) { if (++t > 80000U) return 0; }
    t = 0U;
    while (Gpio_Get(GPIOA, 1U) == 0) { if (++t > 80000U) return 0; }
    t = 0U;
    while (Gpio_Get(GPIOA, 1U) == 1) { if (++t > 80000U) return 0; }

    if (!Dht11_ReadByte(&hI)) return 0;
    if (!Dht11_ReadByte(&hD)) return 0;
    if (!Dht11_ReadByte(&tI)) return 0;
    if (!Dht11_ReadByte(&tD)) return 0;
    if (!Dht11_ReadByte(&cs)) return 0;

    if (((hI + hD + tI + tD) & 0xFFU) != (cs & 0xFFU)) return 0;
    *humidity = (int)hI;
    *temp_c = (int)tI;
    return 1;
}

int main() {
    Clock_EnableGpioA();
    Delay_InitSystick_1ms();
    Usart1_Init_9600_8N1();

    while(1) {
        int t, h;
        if (Dht11_Read(&t, &h)) {
            char buf[64];
            unsigned int n = (unsigned int)sprintf(buf, "DHT11: T=%dC H=%d%%\r\n", t, h);
            (void)n;
            Usart1_WriteString(buf);
        } else {
            Usart1_WriteString("DHT11 read failed\r\n");
        }
        Delay_Ms(1200U);
    }
}
