#include "test_isa.h"

#include "coresim/Decoder.h"
#include "coresim/Opcode.h"

using namespace coresim;

void IsaTests::runTests() {
    // values
    check(static_cast<uint8_t>(Opcode::MOV) == 0x00, "Opcode: MOV, Value: 0x00");
    check(static_cast<uint8_t>(Opcode::MOVI) == 0x40, "Opcode: MOVI, Value: 0x40");

    check(static_cast<uint8_t>(Opcode::ADD) == 0x01, "Opcode: ADD, Value: 0x01");
    check(static_cast<uint8_t>(Opcode::SUB) == 0x02, "Opcode: SUB, Value: 0x02");
    check(static_cast<uint8_t>(Opcode::MUL) == 0x03, "Opcode: MUL, Value: 0x03");
    check(static_cast<uint8_t>(Opcode::DIV) == 0x04, "Opcode: DIV, Value: 0x04");
    check(static_cast<uint8_t>(Opcode::ADDI) == 0x41, "Opcode: ADDI, Value: 0x41");

    check(static_cast<uint8_t>(Opcode::AND) == 0x05, "Opcode: AND, Value: 0x05");
    check(static_cast<uint8_t>(Opcode::OR) == 0x06, "Opcode: OR, Value: 0x06");
    check(static_cast<uint8_t>(Opcode::XOR) == 0x07, "Opcode: XOR, Value: 0x07");
    check(static_cast<uint8_t>(Opcode::NOT) == 0x08, "Opcode: NOT, Value: 0x08");

    check(static_cast<uint8_t>(Opcode::CMP) == 0x09, "Opcode: CMP, Value: 0x09");

    check(static_cast<uint8_t>(Opcode::PUSH) == 0x0A, "Opcode: PUSH, Value: 0x0A");
    check(static_cast<uint8_t>(Opcode::POP) == 0x0B, "Opcode: POP, Value: 0x0B");
    check(static_cast<uint8_t>(Opcode::RET) == 0x0C, "Opcode: RET, Value: 0x0C");
    check(static_cast<uint8_t>(Opcode::CALL) == 0xC5, "Opcode: CALL, Value: 0xC5");

    check(static_cast<uint8_t>(Opcode::NOP) == 0x0D, "Opcode: NOP, Value: 0x0D");
    check(static_cast<uint8_t>(Opcode::HLT) == 0x0E, "Opcode: HLT, Value: 0x0E");

    check(static_cast<uint8_t>(Opcode::OUT) == 0x0F, "Opcode: OUT, Value: 0x0F");

    check(static_cast<uint8_t>(Opcode::LOAD) == 0x80, "Opcode: LOAD, Value: 0x80");
    check(static_cast<uint8_t>(Opcode::STORE) == 0x81, "Opcode: STORE, Value: 0x81");

    check(static_cast<uint8_t>(Opcode::JMP) == 0xC0, "Opcode: JMP, Value: 0xC0");
    check(static_cast<uint8_t>(Opcode::JZ) == 0xC1, "Opcode: JZ, Value: 0xC1");
    check(static_cast<uint8_t>(Opcode::JNZ) == 0xC2, "Opcode: JNZ, Value: 0xC2");
    check(static_cast<uint8_t>(Opcode::JN) == 0xC3, "Opcode: JN, Value: 0xC3");
    check(static_cast<uint8_t>(Opcode::JC) == 0xC4, "Opcode: JC, Value: 0xC4");

    // formats
    check(static_cast<uint8_t>(Opcode::MOV) >> 6 == Decoder::kFormatR, "Opcode: MOV, Format: R");
    check(static_cast<uint8_t>(Opcode::ADD) >> 6 == Decoder::kFormatR, "Opcode: ADD, Format: R");
    check(static_cast<uint8_t>(Opcode::SUB) >> 6 == Decoder::kFormatR, "Opcode: SUB, Format: R");
    check(static_cast<uint8_t>(Opcode::MUL) >> 6 == Decoder::kFormatR, "Opcode: MUL, Format: R");
    check(static_cast<uint8_t>(Opcode::DIV) >> 6 == Decoder::kFormatR, "Opcode: DIV, Format: R");
    check(static_cast<uint8_t>(Opcode::AND) >> 6 == Decoder::kFormatR, "Opcode: AND, Format: R");
    check(static_cast<uint8_t>(Opcode::OR) >> 6 == Decoder::kFormatR, "Opcode: OR, Format: R");
    check(static_cast<uint8_t>(Opcode::XOR) >> 6 == Decoder::kFormatR, "Opcode: XOR, Format: R");
    check(static_cast<uint8_t>(Opcode::NOT) >> 6 == Decoder::kFormatR, "Opcode: NOT, Format: R");
    check(static_cast<uint8_t>(Opcode::CMP) >> 6 == Decoder::kFormatR, "Opcode: CMP, Format: R");
    check(static_cast<uint8_t>(Opcode::PUSH) >> 6 == Decoder::kFormatR, "Opcode: PUSH, Format: R");
    check(static_cast<uint8_t>(Opcode::POP) >> 6 == Decoder::kFormatR, "Opcode: POP, Format: R");
    check(static_cast<uint8_t>(Opcode::RET) >> 6 == Decoder::kFormatR, "Opcode: RET, Format: R");
    check(static_cast<uint8_t>(Opcode::NOP) >> 6 == Decoder::kFormatR, "Opcode: NOP, Format: R");
    check(static_cast<uint8_t>(Opcode::HLT) >> 6 == Decoder::kFormatR, "Opcode: HLT, Format: R");
    check(static_cast<uint8_t>(Opcode::OUT) >> 6 == Decoder::kFormatR, "Opcode: OUT, Format: R");

    check(static_cast<uint8_t>(Opcode::MOVI) >> 6 == Decoder::kFormatI, "Opcode: MOVI, Format: I");
    check(static_cast<uint8_t>(Opcode::ADDI) >> 6 == Decoder::kFormatI, "Opcode: ADDI, Format: I");

    check(static_cast<uint8_t>(Opcode::LOAD) >> 6 == Decoder::kFormatM, "Opcode: LOAD, Format: M");
    check(static_cast<uint8_t>(Opcode::STORE) >> 6 == Decoder::kFormatM, "Opcode: STORE, Format: M");

    check(static_cast<uint8_t>(Opcode::JMP) >> 6 == Decoder::kFormatJ, "Opcode: JMP, Format: J");
    check(static_cast<uint8_t>(Opcode::JZ) >> 6 == Decoder::kFormatJ, "Opcode: JZ, Format: J");
    check(static_cast<uint8_t>(Opcode::JNZ) >> 6 == Decoder::kFormatJ, "Opcode: JNZ, Format: J");
    check(static_cast<uint8_t>(Opcode::JN) >> 6 == Decoder::kFormatJ, "Opcode: JN, Format: J");
    check(static_cast<uint8_t>(Opcode::JC) >> 6 == Decoder::kFormatJ, "Opcode: JC, Format: J");
    check(static_cast<uint8_t>(Opcode::CALL) >> 6 == Decoder::kFormatJ, "Opcode: CALL, Format: J");
}
