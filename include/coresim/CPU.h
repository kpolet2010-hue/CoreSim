#pragma once

#include "coresim/Architecture.h"
#include "coresim/Memory.h"
#include "coresim/Register.h"

#include <array>

namespace coresim {

// 16-bit CPU with 16 general-purpose registers.
class CPU {
public:
    explicit CPU(Memory& memory);

    void reset();

    Register& reg(std::size_t index);
    const Register& reg(std::size_t index) const;

    Address pc() const { return pc_; }
    void setPc(Address address) { pc_ = address & kAddressMask; }

    Memory& memory() { return memory_; }

private:
    Memory& memory_;
    std::array<Register, kRegisterCount> registers_{};
    Address pc_ = 0;
};

} // namespace coresim
