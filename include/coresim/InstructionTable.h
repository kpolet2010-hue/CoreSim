#pragma once

#include "coresim/Opcode.h"

#include <string>
#include <vector>

namespace coresim {

    enum class Operand {
        Rd, 
        Rs1,
        Rs2,
        Imm,
        Mem,
        Target
    };

    struct InstructionInfo {
        const char* name;
        Opcode opcode;
        std::vector<Operand> operands;
    };

    const InstructionInfo* findInstruction(const std::string& mnemonic);

    const InstructionInfo* findInstruction(Opcode opcode);


} // namespace coresim
