; fibonacci.asm - prints the first 10 Fibonacci numbers (0 1 1 2 3 5 8 13 21 34)
; run: coresim examples/fibonacci.asm
;
; R1 = current number, R2 = next number, R3 = counter, R4 = temporary sum

        MOVI R1, 0
        MOVI R2, 1
        MOVI R3, 10
loop:   OUT  R1
        ADD  R4, R1, R2
        MOV  R1, R2
        MOV  R2, R4
        ADDI R3, R3, -1
        JNZ  loop
        HLT
