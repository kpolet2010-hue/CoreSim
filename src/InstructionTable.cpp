#include "coresim/InstructionTable.h"

#include <cctype>

namespace coresim {

    namespace {

        std::string toUpper(std::string s) {
            for (char& c : s) {
                c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            }
            return s;
        }

        const std::vector<InstructionInfo> kInstructions = {
            {"MOV",   Opcode::MOV,   {Operand::Rd, Operand::Rs1}},
            {"MOVI",  Opcode::MOVI,  {Operand::Rd, Operand::Imm}},
            {"ADD",   Opcode::ADD,   {Operand::Rd, Operand::Rs1, Operand::Rs2}},
            {"SUB",   Opcode::SUB,   {Operand::Rd, Operand::Rs1, Operand::Rs2}},
            {"MUL",   Opcode::MUL,   {Operand::Rd, Operand::Rs1, Operand::Rs2}},
            {"DIV",   Opcode::DIV,   {Operand::Rd, Operand::Rs1, Operand::Rs2}},
            {"ADDI",  Opcode::ADDI,  {Operand::Rd, Operand::Rs1, Operand::Imm}},
            {"AND",   Opcode::AND,   {Operand::Rd, Operand::Rs1, Operand::Rs2}},
            {"OR",    Opcode::OR,    {Operand::Rd, Operand::Rs1, Operand::Rs2}},
            {"XOR",   Opcode::XOR,   {Operand::Rd, Operand::Rs1, Operand::Rs2}},
            {"NOT",   Opcode::NOT,   {Operand::Rd, Operand::Rs1}},
            {"CMP",   Opcode::CMP,   {Operand::Rs1, Operand::Rs2}},
            {"PUSH",  Opcode::PUSH,  {Operand::Rs1}},
            {"POP",   Opcode::POP,   {Operand::Rd}},
            {"RET",   Opcode::RET,   {}},
            {"CALL",  Opcode::CALL,  {Operand::Target}},
            {"NOP",   Opcode::NOP,   {}},
            {"HLT",   Opcode::HLT,   {}},
            {"OUT",   Opcode::OUT,   {Operand::Rs1}},
            {"LOAD",  Opcode::LOAD,  {Operand::Rd, Operand::Mem}},
            {"STORE", Opcode::STORE, {Operand::Rd, Operand::Mem}},
            {"JMP",   Opcode::JMP,   {Operand::Target}},
            {"JZ",    Opcode::JZ,    {Operand::Target}},
            {"JNZ",   Opcode::JNZ,   {Operand::Target}},
            {"JN",    Opcode::JN,    {Operand::Target}},
            {"JC",    Opcode::JC,    {Operand::Target}},
        };

    } // namespace

    const InstructionInfo* findInstruction(const std::string& mnemonic) {
        std::string upper = toUpper(mnemonic);
        for (const InstructionInfo& info : kInstructions) {
            if (upper == info.name) {
                return &info;
            }
        }
        return nullptr;
    }

    const InstructionInfo* findInstruction(Opcode opcode) {
        for (const InstructionInfo& info : kInstructions) {
            if (opcode == info.opcode) {
                return &info;
            }
        }
        return nullptr;
    }

} // namespace coresim
