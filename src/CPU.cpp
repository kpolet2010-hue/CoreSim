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

        switch (static_cast<Opcode>(decoded.opcode)) {
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
            executeAlu(AluOp::ADD, decoded.rd, registers_.get(decoded.rs1), registers_.get(decoded.rs2));
            break;
        case Opcode::ADDI:
            executeAlu(AluOp::ADD, decoded.rd, registers_.get(decoded.rs1), decoded.immediate);
            break;
        case Opcode::SUB:
            executeAlu(AluOp::SUB, decoded.rd, registers_.get(decoded.rs1), registers_.get(decoded.rs2));
            break;
        case Opcode::MUL:
            executeAlu(AluOp::MUL, decoded.rd, registers_.get(decoded.rs1), registers_.get(decoded.rs2));
            break;
        case Opcode::DIV:
            executeAlu(AluOp::DIV, decoded.rd, registers_.get(decoded.rs1), registers_.get(decoded.rs2));
            break;
        case Opcode::CMP:
            executeAlu(AluOp::CMP, decoded.rd, registers_.get(decoded.rs1), registers_.get(decoded.rs2));
            break;
        case Opcode::AND:
            executeAlu(AluOp::AND, decoded.rd, registers_.get(decoded.rs1), registers_.get(decoded.rs2));
            break;
        case Opcode::OR:
            executeAlu(AluOp::OR, decoded.rd, registers_.get(decoded.rs1), registers_.get(decoded.rs2));
            break;
        case Opcode::XOR:
            executeAlu(AluOp::XOR, decoded.rd, registers_.get(decoded.rs1), registers_.get(decoded.rs2));
            break;
        case Opcode::NOT:
            executeAlu(AluOp::NOT, decoded.rd, registers_.get(decoded.rs1));
            break;
        case Opcode::JMP:
            state_.setPc(decoded.address);
            break;
        case Opcode::JZ:
            jumpIf(state_.flag(kFlagZ), decoded.address);
            break;
        case Opcode::JNZ:
            jumpIf(!state_.flag(kFlagZ), decoded.address);
            break;
        case Opcode::JN:
            jumpIf(state_.flag(kFlagN), decoded.address);
            break;
        case Opcode::JC:
            jumpIf(state_.flag(kFlagC), decoded.address);
            break;
        case Opcode::LOAD:
            registers_.set(decoded.rd, memory_.read16(effectiveAddress(decoded)));
            break;
        case Opcode::STORE:
            // the rd field holds the register whose value is stored
            memory_.write16(effectiveAddress(decoded), registers_.get(decoded.rd));
            break;
        default:
            break;
        }
    }

    void CPU::executeAlu(AluOp op, std::uint8_t rd, Word a, Word b) {
        // if the alu throws, registers and flags stay unchanged
        AluResult result = alu_.execute(op, a, b);

        if (result.writesResult) {
            registers_.set(rd, result.value);
        }
        state_.setFlag(kFlagZ, result.z);
        state_.setFlag(kFlagN, result.n);
        state_.setFlag(kFlagC, result.c);
        state_.setFlag(kFlagV, result.v);
    }

    void CPU::jumpIf(bool condition, Address target) {
        // pc already points to the next instruction, so a jump that is not taken does nothing
        if (condition) {
            state_.setPc(target);
        }
    }

    Address CPU::effectiveAddress(const DecodedInstruction& decoded) const {
        // wrapping to 16 bits keeps the access inside the bank selected by DBR
        Address offsetInBank = (registers_.get(decoded.rs1) + decoded.offset) & 0xFFFF;
        return (static_cast<Address>(state_.dbr()) << 16) | offsetInBank;
    }

} // namespace coresim
