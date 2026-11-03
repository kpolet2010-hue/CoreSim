#pragma once

#include <cstddef>
#include <cstdint>

namespace coresim {

    // machine word as 16 bit path
    using Word = std::uint16_t;

    // 32 bit instruction
    using Instruction = std::uint32_t;

    // physical address, only 18 bits are used
    // stored as 32 bit because cpp has no 18-bit type
    using Address = std::uint32_t;

    constexpr std::size_t kRegisterCount = 16;

    constexpr unsigned kAddressBits = 18;
    constexpr std::size_t kMemorySize = std::size_t{1} << kAddressBits; // 256 KB
    constexpr Address kAddressMask = static_cast<Address>(kMemorySize - 1);

    constexpr std::size_t kInstructionSize = sizeof(Instruction); // 4 bytes, little-endian

    constexpr const char* kVersion = "0.1";

} // namespace coresim
