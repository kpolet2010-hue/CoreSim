#include "coresim/Memory.h"

#include <algorithm>

namespace coresim {

Memory::Memory() : data_(kMemorySize, 0) {}

void Memory::reset() {
    std::fill(data_.begin(), data_.end(), std::uint8_t{0});
}

std::uint8_t Memory::read8(Address address) const {
    return data_[address & kAddressMask];
}

void Memory::write8(Address address, std::uint8_t value) {
    data_[address & kAddressMask] = value;
}

std::uint32_t Memory::read32(Address address) const {
    std::uint32_t value = 0;
    for (unsigned i = 0; i < 4; ++i) {
        value |= static_cast<std::uint32_t>(read8(address + i)) << (8 * i);
    }
    return value;
}

void Memory::write32(Address address, std::uint32_t value) {
    for (unsigned i = 0; i < 4; ++i) {
        write8(address + i, static_cast<std::uint8_t>(value >> (8 * i)));
    }
}

} // namespace coresim
