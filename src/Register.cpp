#include "coresim/Register.h"

#include <stdexcept>

namespace coresim {

Word Register::get(std::size_t index) const {
    checkIndex(index);
    return registers_[index];
}

void Register::set(std::size_t index, Word value) {
    checkIndex(index);
    registers_[index] = value;
}

void Register::reset() {
    registers_.fill(0);
}

void Register::checkIndex(std::size_t index) const {
    if (index >= kRegisterCount) {
        throw std::out_of_range("Register index out of range");
    }
}

} // namespace coresim
