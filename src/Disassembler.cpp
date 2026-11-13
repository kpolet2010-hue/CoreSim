#include "coresim/Disassembler.h"

#include "coresim/Decoder.h"
#include "coresim/InstructionTable.h"
#include "coresim/Opcode.h"

#include <iomanip>
#include <sstream>
#include <cstdint>

namespace coresim {

    std::string disassemble(Instruction instruction) {
        Decoder decoder;
        DecodedInstruction decoded = decoder.decode(instruction);

        const InstructionInfo* info = findInstruction(static_cast<Opcode>(decoded.opcode));

        if (info == nullptr) {
            std::ostringstream hex;
            hex << "??? 0x"
                << std::hex
                << std::uppercase
                << std::setw(8)
                << std::setfill('0')
                << instruction;
            return hex.str();
        }


        std::string text = info->name;

        for (std::size_t i = 0; i < info->operands.size(); ++i) {
            text += (i == 0) ? " " : ", ";           // separator

            switch (info->operands[i]) {
                case Operand::Rd:
                    text += "R" + std::to_string(decoded.rd);
                    break;
                case Operand::Rs1:
                    text += "R" + std::to_string(decoded.rs1);
                    break;
                case Operand::Rs2:
                    text += "R" + std::to_string(decoded.rs2);
                    break;
                case Operand::Imm:
                    text += std::to_string(static_cast<int16_t>(decoded.immediate));
                    break;
                case Operand::Mem:
                    text += "[R" + std::to_string(decoded.rs1);
                    if (decoded.offset > 0) {
                        text += " + " + std::to_string(decoded.offset);
                    } else if (decoded.offset < 0) {
                        text += " - " + std::to_string(-decoded.offset);
                    }
                    text += "]";
                    break;
                case Operand::Target: {
                    std::ostringstream target;
                    target << "0x"
                           << std::hex
                           << std::uppercase
                           << std::setw(5)
                           << std::setfill('0')
                           << decoded.address;
                    text += target.str();
                    break;
                }
                default:
                    break;
            }
        }
        return text;
    }

} // namespace coresim
