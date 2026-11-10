; factorial.asm - calculates 5! = 120
; run: coresim examples/factorial.asm
;
; R1 = counter (5, 4, ... 1), R2 = result

        MOVI R1, 5
        MOVI R2, 1
loop:   MUL  R2, R2, R1
        ADDI R1, R1, -1
        JNZ  loop
        OUT  R2
        HLT
