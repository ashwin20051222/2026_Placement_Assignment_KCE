        AREA    |.text|, CODE, READONLY
        THUMB

        EXPORT  Add32_StoreResult

Add32_StoreResult
        ADDS    R0, R0, R1
        STR     R0, [R2]
        BX      LR

        END
