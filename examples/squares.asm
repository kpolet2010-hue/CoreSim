; squares.asm - prints the squares of 1 to 5 (1 4 9 16 25) with a subroutine
; run: coresim examples/squares.asm
;
; R1 = n, R5 = limit, R2 = used by the subroutine and saved on the stack

        MOVI R1, 1
        MOVI R5, 6
loop:   CALL square
        ADDI R1, R1, 1
        CMP  R1, R5
        JC   loop               ; while R1 < R5
        HLT

; square: prints R1 * R1, keeps R2 unchanged
square: PUSH R2
        MUL  R2, R1, R1
        OUT  R2
        POP  R2
        RET
