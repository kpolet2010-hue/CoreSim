#pragma once

#include "coresim/Architecture.h"

namespace coresim {

// A single 16-bit general-purpose register.
class Register {
public:
    Register() = default;

    Word get() const { return value_; }
    void set(Word value) { value_ = value; }
    void reset() { value_ = 0; }

private:
    Word value_ = 0;
};

} // namespace coresim
