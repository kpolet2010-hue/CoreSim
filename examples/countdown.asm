; countdown.asm - counts down from 3 to 1
; run: coresim examples/countdown.asm

        MOVI R1, 3
loop:   OUT  R1
        ADDI R1, R1, -1
        JNZ  loop
        HLT
