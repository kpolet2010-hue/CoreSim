#pragma once

#include <cstdint>

namespace coresim {
    struct DecodedInstruction
    {
        uint8_t opcode;

        uint8_t rd;
        uint8_t rs1;
        uint8_t rs2;

        uint16_t immediate;
        uint32_t address;

        int32_t offset;
    };
    
}