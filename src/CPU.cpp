#include "coresim/CPU.h"
#include "coresim/Opcode.h"

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
        default:
            break;
        }
    }

} // namespace coresim
