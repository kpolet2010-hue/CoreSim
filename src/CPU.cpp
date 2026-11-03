#include "coresim/CPU.h"

namespace coresim {

    CPU::CPU(Memory& memory) : memory_(memory) {}

    void CPU::reset() {
        registers_.reset();
        state_.reset();
    }

} // namespace coresim
