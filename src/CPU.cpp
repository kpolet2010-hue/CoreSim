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
            std::uint32_t sum = a + b;
            Word result = sum & 0xFFFF;
            
            registers_.set(decoded.rd, result);
            break;
        }
        default:
            break;
        }
    }

} // namespace coresim
