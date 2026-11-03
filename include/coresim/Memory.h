#pragma once

#include "coresim/Architecture.h"

#include <cstdint>
#include <vector>

namespace coresim {

// 256 kb ram, addressed byte by byte
// uses little endian byte order and throws out of range on invalid access
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