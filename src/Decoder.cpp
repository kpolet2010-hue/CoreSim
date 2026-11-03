#include "coresim/Decoder.h"
#include "coresim/Architecture.h"

namespace coresim {

    DecodedInstruction Decoder::decode(uint32_t instruction) {
        DecodedInstruction decoded{};

        decoded.opcode = (instruction >> 24) & 0xFF;
        Format format  = static_cast<Format>(decoded.opcode >> 6);

        switch (format) {
            case kFormatR:
                decoded.rd  = (instruction >> 20) & 0x0F;
                decoded.rs1 = (instruction >> 16) & 0x0F;
                decoded.rs2 = (instruction >> 12) & 0x0F;
                break;

            case kFormatI:
                decoded.rd        = (instruction >> 20) & 0x0F;
                decoded.rs1       = (instruction >> 16) & 0x0F;
                decoded.immediate = instruction & 0xFFFF;
                break;

            case kFormatM:
                decoded.rd     = (instruction >> 20) & 0x0F;
                decoded.rs1    = (instruction >> 16) & 0x0F;
                decoded.offset = static_cast<int16_t>(instruction & 0xFFFF);
                break;

            case kFormatJ:
                decoded.address = instruction & kAddressMask;
                break;
        }

        return decoded;
    }

    
}