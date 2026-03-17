        AREA    |.text|, CODE, READONLY
        THUMB

        EXPORT  Fib10_Fill

Fib10_Fill
        MOVS    R1, #0
        STR     R1, [R0, #0]
        MOVS    R2, #1
        STR     R2, [R0, #4]

        MOVS    R3, #2
loop
        LDR     R1, [R0, R3, LSL #2]

        SUBS    R4, R3, #1
        LDR     R1, [R0, R4, LSL #2]
        SUBS    R5, R3, #2
        LDR     R2, [R0, R5, LSL #2]
        ADDS    R1, R1, R2
        STR     R1, [R0, R3, LSL #2]
        ADDS    R3, R3, #1
        CMP     R3, #10
        BLT     loop
        BX      LR

        END
