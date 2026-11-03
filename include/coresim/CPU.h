#pragma once

#include "coresim/CPUState.h"
#include "coresim/Memory.h"
#include "coresim/Register.h"
#include "coresim/Decoder.h"

namespace coresim {

// 16 bit cpu with 16 general purpose registers
    class CPU {
        public:
            explicit CPU(Memory& memory);

            void reset();
            void step();
            bool halted() const { return halted_; }
            Memory& memory() { return memory_; }

            Register& registers() { return registers_; }
            const Register& registers() const { return registers_; }

            CPUState& state() { return state_; }
            const CPUState& state() const { return state_; }

        private:
            Memory& memory_;
            Register registers_;
            CPUState state_;
            Decoder decoder_;
            bool halted_ = false;
    };

} // namespace coresim
