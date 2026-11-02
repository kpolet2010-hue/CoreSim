#include "coresim/Decoder.h"

namespace coresim {

    DecodedInstruction Decoder::decode(uint32_t instruction) {
        
        DecodedInstruction decoded{};
        
        decoded.opcode = (instruction >> 24) & 0xFF;
        decoded.rd = (instruction >> 20) & 0x0F;
        decoded.rs1 = (instruction >> 16) & 0x0F;
        decoded.rs2 = (instruction >> 12) & 0x0F;

        return decoded;
    
    }
    
}