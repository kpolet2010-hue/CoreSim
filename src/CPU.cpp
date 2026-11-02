#include "coresim/CPU.h"

namespace coresim {

CPU::CPU(Memory& memory) : memory_(memory) {}

void CPU::reset() {
    registers_.reset();
    pc_ = 0;
}

} // namespace coresim
