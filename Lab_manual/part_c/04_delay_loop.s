        AREA    |.text|, CODE, READONLY
        THUMB

        EXPORT  Delay_Loop

Delay_Loop
        CMP     R0, #0
        BEQ     done
loop
        SUBS    R0, R0, #1
        BNE     loop
done
        BX      LR

        END
