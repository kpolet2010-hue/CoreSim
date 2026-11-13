#include "coresim/Disassembler.h"

#include "coresim/Decoder.h"
#include "coresim/InstructionTable.h"

namespace coresim {

    std::string disassemble(Instruction instruction) {
        // TODO: your logic, step by step
        //  1. decode the instruction with Decoder
        //  2. find the table entry with findInstruction(opcode), unknown opcode -> "??? 0x...."
        //  3. start the text with the name
        //  4. append every operand of the entry (register, imm, [rs1 +/- offset], target)
        (void)instruction;      // remove this line once 'instruction' is used
        return "";
    }

} // namespace coresim
