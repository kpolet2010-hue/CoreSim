#pragma once

#include "coresim/Architecture.h"

#include <string>

namespace coresim {

    // turns one 32-bit instruction back into assembly text, see docs/DISASSEMBLER.md
    // never throws, an unknown opcode gives "??? 0x" and the 8 hex digits of the instruction
    std::string disassemble(Instruction instruction);

} // namespace coresim
