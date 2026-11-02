#include "coresim/CPU.h"

namespace coresim {

CPU::CPU(Memory& memory) : memory_(memory) {}

void CPU::reset() {
    for (auto& r : registers_) {
        r.reset();
    }
    pc_ = 0;
}

Register& CPU::reg(std::size_t index) {
    return registers_.at(index);
}

const Register& CPU::reg(std::size_t index) const {
    return registers_.at(index);
}

} // namespace coresim
