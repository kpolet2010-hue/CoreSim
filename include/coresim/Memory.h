#pragma once

#include "coresim/Architecture.h"

#include <cstdint>
#include <vector>

namespace coresim {

// 256 KB byte-addressable RAM with 18-bit addressing.
// Multi-byte accesses are little-endian.
// Out-of-range accesses throw std::out_of_range.
class Memory {
public:
    Memory();

    void reset();

    std::uint8_t read8(Address address) const;
    void write8(Address address, std::uint8_t value);

    std::uint32_t read32(Address address) const;
    void write32(Address address, std::uint32_t value);

    std::size_t size() const { return data_.size(); }

private:
    void checkRange(Address address, std::size_t size) const;

    std::vector<std::uint8_t> data_;
};

} // namespace coresim