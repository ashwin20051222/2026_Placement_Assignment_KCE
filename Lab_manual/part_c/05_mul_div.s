        AREA    |.text|, CODE, READONLY
        THUMB

        EXPORT  MulDiv

MulDiv
        MUL     R4, R0, R1
        STR     R4, [R2]
        CMP     R1, #0
        BEQ     div_zero
        UDIV    R5, R0, R1
        STR     R5, [R3]
        BX      LR
div_zero
        MOVS    R5, #0
        STR     R5, [R3]
        BX      LR

        END
