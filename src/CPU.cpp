#include "coresim/CPU.h"

namespace coresim {

    CPU::CPU(Memory& memory) : memory_(memory) {}

    void CPU::reset() {
        registers_.reset();
        state_.reset();
    }

    void CPU::step() {
        Instruction instruction = memory_.read32(state_.pc());          // read line where space is
        state_.setIr(instruction);                                      // memorize what was read
        state_.setPc(state_.pc() + 4);                                  // space 4 bytes further
    }

} // namespace coresim
