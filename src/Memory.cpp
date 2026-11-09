#include "coresim/Memory.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace coresim {

    Memory::Memory() : data_(kMemorySize, 0) {}

    void Memory::reset() {
        std::fill(data_.begin(), data_.end(), std::uint8_t{0});
    }

    std::uint8_t Memory::read8(Address address) const {
        checkRange(address, 1);
        return data_[address];
    }

    void Memory::write8(Address address, std::uint8_t value) {
        checkRange(address, 1);
        data_[address] = value;
    }

    void Memory::write32(Address address, std::uint32_t value) {
        checkRange(address, 4);
        for (unsigned i = 0; i < 4; ++i) {
            write8(address + i, static_cast<std::uint8_t>(value >> (8 * i)));
        }
    }

    std::uint32_t Memory::read32(Address address) const {
        checkRange(address, 4);
        std::uint32_t value = 0;
        for (unsigned i = 0; i < 4; ++i) {
            value |= static_cast<std::uint32_t>(read8(address + i)) << (8 * i);
        }
        return value;
    }

    Word Memory::read16(Address address) const {
        checkRange(address, 2);
        Word value = 0;
        for (unsigned i = 0; i < 2; ++i) {
            value |= static_cast<std::uint32_t>(read8(address + i)) << (8 * i);
        }
        return value;
    }

    void Memory::write16(Address address, Word value) {
        checkRange(address, 2);
        for (unsigned i = 0; i < 2; ++i) {
            write8(address + i, static_cast<std::uint8_t>(value >> (8 * i)));
        }
    }

    void Memory::checkRange(Address address, std::size_t size) const {
        if (address >= kMemorySize || size > kMemorySize - address) {
            throw std::out_of_range("Memory access out of bounds");
        }
    }

} // namespace coresim