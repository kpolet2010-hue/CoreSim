#include "coresim/CPU.h"
#include "coresim/Opcode.h"

#include <cstdint>

namespace coresim {

    CPU::CPU(Memory& memory) : memory_(memory) {}

    void CPU::reset() {
        registers_.reset();
        state_.reset();
        halted_ = false;
    }

    void CPU::step() {
        if (halted_) {
            return;
        }

        Instruction instruction = memory_.read32(state_.pc());
        state_.setIr(instruction);
        state_.setPc(state_.pc() + 4);

        DecodedInstruction decoded = decoder_.decode(instruction);

        switch (static_cast<Opcode>(decoded.opcode)) 
        {
        case Opcode::NOP:
            break;
        case Opcode::HLT:
            halted_ = true;
            break;
        case Opcode::MOV:
            registers_.set(decoded.rd, registers_.get(decoded.rs1));
            break;
        case Opcode::MOVI:
            registers_.set(decoded.rd, decoded.immediate);
            break;
        case Opcode::ADD: {
            Word a = registers_.get(decoded.rs1);
            Word b = registers_.get(decoded.rs2);
            Word result = addWithFlags(a, b);
            registers_.set(decoded.rd, result);
            break;
        }
        case Opcode::ADDI: {
            Word a = registers_.get(decoded.rs1);
            Word b = decoded.immediate;
            Word result = addWithFlags(a, b);
            registers_.set(decoded.rd, result);
            break;
        }
        case Opcode::SUB: {
            Word a = registers_.get(decoded.rs1);
            Word b = registers_.get(decoded.rs2);
            Word result = subWithFlags(a, b);
            registers_.set(decoded.rd, result);
            break;
        }
        default:
            break;
        }
    }

    Word CPU::addWithFlags(Word a, Word b) {
        Word result = (a + b) & 0xFFFF;
        std::uint32_t sum = a + b;
        state_.setFlag(kFlagZ, result == 0);
        state_.setFlag(kFlagN, (result >> 15) & 1);
        state_.setFlag(kFlagC, sum > 0xFFFF);
        state_.setFlag(kFlagV, ((a ^ result) & (b ^ result) & 0x8000) != 0);
        return result;
    }

    Word CPU::subWithFlags(Word a, Word b) {
        Word result = (a - b) & 0xFFFF;
        state_.setFlag(kFlagZ, result == 0);
        state_.setFlag(kFlagN, (result >> 15) & 1);
        state_.setFlag(kFlagC, a < b);
        state_.setFlag(kFlagV, ((a ^ b) & (a ^ result) & 0x8000) != 0);
        return result;
    }

} // namespace coresim
