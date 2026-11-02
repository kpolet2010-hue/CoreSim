#pragma once

#include "coresim/Architecture.h"

#include <array>
#include <cstddef>

namespace coresim {

// 16 general-purpose 16-bit registers (R0–R15).
// Accessing an index >= kRegisterCount throws std::out_of_range.
class Register {
public:
    Word get(std::size_t index) const;
    void set(std::size_t index, Word value);

    void reset();

private:
    void checkIndex(std::size_t index) const;

    std::array<Word, kRegisterCount> registers_{};
};

} // namespace coresim
