#pragma once

#include "coresim/Architecture.h"

namespace coresim {

    // ADD and ADDI both use ADD, they only differ in where the cpu takes b from
    enum class AluOp {
        ADD,
        SUB,
        MUL,
        DIV,
        CMP,
        AND,
        OR,
        XOR,
        NOT
    };

    struct AluResult {
        Word value;         // 16 bit result, for CMP the difference a - b
        bool z;             // Zero
        bool n;             // Negative
        bool c;             // Carry
        bool v;             // Overflow
        bool writesResult;  // false if value must not be stored (CMP)
    };

    // calculates results and flags, has no state and no access to the cpu
    class ALU {
        public:
            // b is ignored by NOT
            // throws std::runtime_error on DIV by zero
            AluResult execute(AluOp op, Word a, Word b = 0) const;
    };

} // namespace coresim
