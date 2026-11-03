#pragma once

#include <cstdint>

#include "DecodedInstruction.h"

namespace coresim {

    class Decoder {
        public:
            DecodedInstruction decode(uint32_t instruction);

            enum Format : uint8_t {
                kFormatR = 0,   // opcode 0x00 - 0x3F
                kFormatI = 1,   // opcode 0x40 - 0x7F
                kFormatM = 2,   // opcode 0x80 - 0xBF
                kFormatJ = 3    // opcode 0xC0 - 0xFF
            };
    };
}