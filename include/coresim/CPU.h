#pragma once

#include "coresim/ALU.h"
#include "coresim/CPUState.h"
#include "coresim/Memory.h"
#include "coresim/Register.h"
#include "coresim/Decoder.h"
#include <iostream>

namespace coresim {

// 16 bit cpu with 16 general purpose registers
    class CPU {
        public:
            explicit CPU(Memory& memory, std::ostream& output = std::cout);

            void reset();
            void step();
            bool run(std::size_t maxSteps);
            bool halted() const { return halted_; }
            Memory& memory() { return memory_; }
            std::ostream& output() { return output_; }

            Register& registers() { return registers_; }
            const Register& registers() const { return registers_; }

            CPUState& state() { return state_; }
            const CPUState& state() const { return state_; }

        private:
            Memory& memory_;
            std::ostream& output_;
            Register registers_;
            CPUState state_;
            Decoder decoder_;
            ALU alu_;
            bool halted_ = false;

            // runs the alu on a and b, writes the result to R[rd] and updates the flags
            void executeAlu(AluOp op, std::uint8_t rd, Word a, Word b = 0);
            // sets the pc to target if condition is true
            void jumpIf(bool condition, Address target);
            // (DBR << 16) | ((R[rs1] + offset) & 0xFFFF), used by LOAD and STORE
            Address effectiveAddress(const DecodedInstruction& decoded) const;
    };

} // namespace coresim
