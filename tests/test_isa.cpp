#include "test_isa.h"

#include "coresim/Decoder.h"
#include "coresim/InstructionTable.h"
#include "coresim/Opcode.h"

#include <string>
#include <vector>

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

    // operand encoding: fields land where the ISA conventions expect them
    Decoder decoder;

    // STORE R3, [R2 - 4]: value register rs is encoded in the rd field
    DecodedInstruction store = decoder.decode(0x8132FFFC);
    check(store.opcode == static_cast<uint8_t>(Opcode::STORE), "Decoded Value: 0x8132FFFC, Opcode: STORE");
    check(store.rd == 3, "Decoded Value: 0x8132FFFC, Rs (rd field): 3");
    check(store.rs1 == 2, "Decoded Value: 0x8132FFFC, Rs1 (base): 2");
    check(store.offset == -4, "Decoded Value: 0x8132FFFC, Offset: -4");
    check(store.rs2 == 0, "Decoded Value: 0x8132FFFC, Rs2: 0 (no rs2 in M-format)");

    // LOAD R1, [R2 + 8]
    DecodedInstruction load = decoder.decode(0x80120008);
    check(load.opcode == static_cast<uint8_t>(Opcode::LOAD), "Decoded Value: 0x80120008, Opcode: LOAD");
    check(load.rd == 1, "Decoded Value: 0x80120008, Rd: 1");
    check(load.rs1 == 2, "Decoded Value: 0x80120008, Rs1 (base): 2");
    check(load.offset == 8, "Decoded Value: 0x80120008, Offset: 8");

    // PUSH R5 uses rs1, POP R7 uses rd
    DecodedInstruction push = decoder.decode(0x0A050000);
    check(push.opcode == static_cast<uint8_t>(Opcode::PUSH), "Decoded Value: 0x0A050000, Opcode: PUSH");
    check(push.rs1 == 5, "Decoded Value: 0x0A050000, Rs1: 5");
    check(push.rd == 0, "Decoded Value: 0x0A050000, Rd: 0 (unused)");

    DecodedInstruction pop = decoder.decode(0x0B700000);
    check(pop.opcode == static_cast<uint8_t>(Opcode::POP), "Decoded Value: 0x0B700000, Opcode: POP");
    check(pop.rd == 7, "Decoded Value: 0x0B700000, Rd: 7");
    check(pop.rs1 == 0, "Decoded Value: 0x0B700000, Rs1: 0 (unused)");

    // MOVI R1, 0x1234
    DecodedInstruction movi = decoder.decode(0x40101234);
    check(movi.opcode == static_cast<uint8_t>(Opcode::MOVI), "Decoded Value: 0x40101234, Opcode: MOVI");
    check(movi.rd == 1, "Decoded Value: 0x40101234, Rd: 1");
    check(movi.immediate == 0x1234, "Decoded Value: 0x40101234, Immediate: 0x1234");

    // CALL 0x00100: absolute 18-bit address
    DecodedInstruction call = decoder.decode(0xC5000100);
    check(call.opcode == static_cast<uint8_t>(Opcode::CALL), "Decoded Value: 0xC5000100, Opcode: CALL");
    check(call.address == 0x00100, "Decoded Value: 0xC5000100, Address: 0x00100");

    // HLT: all register fields unused and 0
    DecodedInstruction hlt = decoder.decode(0x0E000000);
    check(hlt.opcode == static_cast<uint8_t>(Opcode::HLT), "Decoded Value: 0x0E000000, Opcode: HLT");
    check(hlt.rd == 0 && hlt.rs1 == 0 && hlt.rs2 == 0, "Decoded Value: 0x0E000000, Rd/Rs1/Rs2: 0 (unused)");

    // instruction table: every opcode is found by its name and by its value
    struct NamedOpcode {
        const char* name;
        Opcode opcode;
    };
    const std::vector<NamedOpcode> all = {
        {"MOV", Opcode::MOV},   {"MOVI", Opcode::MOVI}, {"ADD", Opcode::ADD},     {"SUB", Opcode::SUB},
        {"MUL", Opcode::MUL},   {"DIV", Opcode::DIV},   {"ADDI", Opcode::ADDI},   {"AND", Opcode::AND},
        {"OR", Opcode::OR},     {"XOR", Opcode::XOR},   {"NOT", Opcode::NOT},     {"CMP", Opcode::CMP},
        {"PUSH", Opcode::PUSH}, {"POP", Opcode::POP},   {"RET", Opcode::RET},     {"CALL", Opcode::CALL},
        {"NOP", Opcode::NOP},   {"HLT", Opcode::HLT},   {"OUT", Opcode::OUT},     {"LOAD", Opcode::LOAD},
        {"STORE", Opcode::STORE}, {"JMP", Opcode::JMP}, {"JZ", Opcode::JZ},       {"JNZ", Opcode::JNZ},
        {"JN", Opcode::JN},     {"JC", Opcode::JC},
    };
    check(all.size() == 26, "InstructionTable: the test covers all 26 instructions");

    for (const NamedOpcode& entry : all) {
        const std::string name = entry.name;

        const InstructionInfo* byName = findInstruction(name);
        check(byName != nullptr && byName->opcode == entry.opcode,
              ("InstructionTable: " + name + " is found by name with the right opcode").c_str());

        const InstructionInfo* byOpcode = findInstruction(entry.opcode);
        check(byOpcode != nullptr && byOpcode->name == name,
              ("InstructionTable: " + name + " is found by opcode with the right name").c_str());
        check(byOpcode == byName,
              ("InstructionTable: " + name + " by name and by opcode is the same entry").c_str());
    }

    // instruction table: operands of one instruction per kind
    const InstructionInfo* addi = findInstruction(Opcode::ADDI);
    check(addi != nullptr && addi->operands == std::vector<Operand>{Operand::Rd, Operand::Rs1, Operand::Imm},
          "InstructionTable: ADDI takes rd, rs1, imm");
    const InstructionInfo* cmp = findInstruction(Opcode::CMP);
    check(cmp != nullptr && cmp->operands == std::vector<Operand>{Operand::Rs1, Operand::Rs2},
          "InstructionTable: CMP takes rs1, rs2 (no rd)");
    const InstructionInfo* storeInfo = findInstruction(Opcode::STORE);
    check(storeInfo != nullptr && storeInfo->operands == std::vector<Operand>{Operand::Rd, Operand::Mem},
          "InstructionTable: STORE takes rd (value register), mem");
    const InstructionInfo* hltInfo = findInstruction(Opcode::HLT);
    check(hltInfo != nullptr && hltInfo->operands.empty(), "InstructionTable: HLT takes no operands");
    const InstructionInfo* jnz = findInstruction(Opcode::JNZ);
    check(jnz != nullptr && jnz->operands == std::vector<Operand>{Operand::Target}, "InstructionTable: JNZ takes a target");

    // instruction table: letter case of the name is ignored
    check(findInstruction("addi") == addi, "InstructionTable: lower case name is found");
    check(findInstruction("AdDi") == addi, "InstructionTable: mixed case name is found");

    // instruction table: unknown names and opcodes give nullptr
    check(findInstruction("MOVE") == nullptr, "InstructionTable: unknown name gives nullptr");
    check(findInstruction("") == nullptr, "InstructionTable: empty name gives nullptr");
    check(findInstruction(static_cast<Opcode>(0x10)) == nullptr, "InstructionTable: unused opcode 0x10 gives nullptr");
    check(findInstruction(static_cast<Opcode>(0x42)) == nullptr, "InstructionTable: unused opcode 0x42 gives nullptr");
    check(findInstruction(static_cast<Opcode>(0xFF)) == nullptr, "InstructionTable: unused opcode 0xFF gives nullptr");
}
