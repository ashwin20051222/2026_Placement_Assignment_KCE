#include "part_c_common.h"

void I2c1_Init_100kHz(void);
void I2c1_WriteByte(unsigned char addr7, unsigned char data);

void I2c1_Init_100kHz(void) {
    Clock_EnableGpioB();
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    {
        unsigned int v = GPIOB->CRL;
        v &= ~(0xFU << (6U * 4U));
        v |= (0xFU << (6U * 4U));
        v &= ~(0xFU << (7U * 4U));
        v |= (0xFU << (7U * 4U));
        GPIOB->CRL = v;
    }

    I2C1->CR2 = 36U;
    I2C1->CCR = 180U;
    I2C1->TRISE = 37U;
    I2C1->CR1 = I2C_CR1_PE;
}

void I2c1_WriteByte(unsigned char addr7, unsigned char data) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB)) {}
    (void)I2C1->SR1;
    I2C1->DR = (unsigned int)(addr7 << 1);
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {}
    (void)I2C1->SR1;
    (void)I2C1->SR2;
    while (!(I2C1->SR1 & I2C_SR1_TXE)) {}
    I2C1->DR = data;
    while (!(I2C1->SR1 & I2C_SR1_BTF)) {}
    I2C1->CR1 |= I2C_CR1_STOP;
}

int main() {
    Delay_InitSystick_1ms();
    I2c1_Init_100kHz();

    while(1) {

        I2c1_WriteByte(0x27U, 0x00U);
        Delay_Ms(500U);
    }
}
