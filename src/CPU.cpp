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
            break;
        case Opcode::MOV:
            registers_.set(decoded.rd, registers_.get(decoded.rs1));
            break;
        case Opcode::MOVI:
            registers_.set(decoded.rd, decoded.immediate);
            break;
        case Opcode::ADD:
            executeAlu(AluOp::ADD, decoded, registers_.get(decoded.rs2));
            break;
        case Opcode::ADDI:
            executeAlu(AluOp::ADDI, decoded, decoded.immediate);
            break;
        case Opcode::SUB:
            executeAlu(AluOp::SUB, decoded, registers_.get(decoded.rs2));
            break;
        case Opcode::MUL:
            executeAlu(AluOp::MUL, decoded, registers_.get(decoded.rs2));
            break;
        case Opcode::DIV:
            executeAlu(AluOp::DIV, decoded, registers_.get(decoded.rs2));
            break;
        case Opcode::CMP:
            executeAlu(AluOp::CMP, decoded, registers_.get(decoded.rs2));
            break;
        case Opcode::AND:
            executeAlu(AluOp::AND, decoded, registers_.get(decoded.rs2));
            break;
        case Opcode::OR:
            executeAlu(AluOp::OR, decoded, registers_.get(decoded.rs2));
            break;
        case Opcode::XOR:
            executeAlu(AluOp::XOR, decoded, registers_.get(decoded.rs2));
            break;
        case Opcode::NOT:
            executeAlu(AluOp::NOT, decoded);
            break;
        default:
            break;
        }
    }

    void CPU::executeAlu(AluOp op, const DecodedInstruction& decoded, Word b) {
        // if the alu throws, registers and flags stay unchanged
        AluResult result = alu_.execute(op, registers_.get(decoded.rs1), b);

        if (result.writesResult) {
            registers_.set(decoded.rd, result.value);
        }
        state_.setFlag(kFlagZ, result.z);
        state_.setFlag(kFlagN, result.n);
        state_.setFlag(kFlagC, result.c);
        state_.setFlag(kFlagV, result.v);
    }

} // namespace coresim
