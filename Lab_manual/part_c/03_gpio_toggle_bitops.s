        AREA    |.text|, CODE, READONLY
        THUMB

        EXPORT  Gpio_Toggle_BSRR

Gpio_Toggle_BSRR
        LDR     R3, [R0]
        TST     R3, R2
        BEQ     setpin
        LSLS    R2, R2, #16
        STR     R2, [R1]
        BX      LR
setpin
        STR     R2, [R1]
        BX      LR

        END
