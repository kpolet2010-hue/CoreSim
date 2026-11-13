#include "test_disassembler.h"

#include "coresim/Assembler.h"
#include "coresim/Disassembler.h"

#include <sstream>
#include <string>
#include <vector>

using namespace coresim;

namespace {

    // checks the text and writes the actual text into the message if it differs
    std::string describe(Instruction instruction, const std::string& expected) {
        std::ostringstream message;
        message << "0x" << std::hex << std::uppercase;
        message.width(8);
        message.fill('0');
        message << instruction << " -> '" << expected << "'";
        std::string actual = disassemble(instruction);
        if (actual != expected) {
            message << " (got '" << actual << "')";
        }
        return message.str();
    }

    // assembles one line and returns its instruction, or 0xFFFFFFFF if it does not give exactly one
    Instruction assembleLine(const std::string& line) {
        std::istringstream input(line);
        try {
            std::vector<Instruction> program = assemble(input);
            return program.size() == 1 ? program[0] : 0xFFFFFFFF;
        } catch (...) {
            return 0xFFFFFFFF;
        }
    }

} // namespace

void DisassemblerTests::runTests() {
    struct Example {
        Instruction instruction;
        const char* text;
    };

    // the examples from DISASSEMBLER.md and one instruction per remaining opcode
    const std::vector<Example> examples = {
        {0x00210000, "MOV R2, R1"},
        {0x40100003, "MOVI R1, 3"},
        {0x01412000, "ADD R4, R1, R2"},
        {0x02312000, "SUB R3, R1, R2"},
        {0x03312000, "MUL R3, R1, R2"},
        {0x04312000, "DIV R3, R1, R2"},
        {0x4111FFFF, "ADDI R1, R1, -1"},
        {0x05312000, "AND R3, R1, R2"},
        {0x06312000, "OR R3, R1, R2"},
        {0x07312000, "XOR R3, R1, R2"},
        {0x08310000, "NOT R3, R1"},
        {0x09012000, "CMP R1, R2"},
        {0x0A010000, "PUSH R1"},
        {0x0B200000, "POP R2"},
        {0x0C000000, "RET"},
        {0xC5000100, "CALL 0x00100"},
        {0x0D000000, "NOP"},
        {0x0E000000, "HLT"},
        {0x0F010000, "OUT R1"},
        {0x80310008, "LOAD R3, [R1 + 8]"},
        {0x8031FFFC, "LOAD R3, [R1 - 4]"},
        {0x81210000, "STORE R2, [R1]"},
        {0xC0000100, "JMP 0x00100"},
        {0xC1000100, "JZ 0x00100"},
        {0xC2000004, "JNZ 0x00004"},
        {0xC3000100, "JN 0x00100"},
        {0xC4000100, "JC 0x00100"},
    };
    for (const Example& example : examples) {
        check(disassemble(example.instruction) == example.text,
              ("disassemble: " + describe(example.instruction, example.text)).c_str());
    }

    // registers: the whole 4-bit field
    check(disassemble(0x01FED000) == "ADD R15, R14, R13", ("registers: " + describe(0x01FED000, "ADD R15, R14, R13")).c_str());
    check(disassemble(0x01000000) == "ADD R0, R0, R0", ("registers: " + describe(0x01000000, "ADD R0, R0, R0")).c_str());

    // imm: signed decimal
    check(disassemble(0x40100000) == "MOVI R1, 0", ("imm: " + describe(0x40100000, "MOVI R1, 0")).c_str());
    check(disassemble(0x40107FFF) == "MOVI R1, 32767", ("imm: " + describe(0x40107FFF, "MOVI R1, 32767")).c_str());
    check(disassemble(0x40108000) == "MOVI R1, -32768", ("imm: " + describe(0x40108000, "MOVI R1, -32768")).c_str());
    check(disassemble(0x4010FFFF) == "MOVI R1, -1", ("imm: " + describe(0x4010FFFF, "MOVI R1, -1")).c_str());
    check(disassemble(0x41120005) == "ADDI R1, R2, 5", ("imm: " + describe(0x41120005, "ADDI R1, R2, 5")).c_str());

    // offset: signed inside the brackets, 0 without
    check(disassemble(0x80310000) == "LOAD R3, [R1]", ("offset: " + describe(0x80310000, "LOAD R3, [R1]")).c_str());
    check(disassemble(0x80127FFF) == "LOAD R1, [R2 + 32767]", ("offset: " + describe(0x80127FFF, "LOAD R1, [R2 + 32767]")).c_str());
    check(disassemble(0x80128000) == "LOAD R1, [R2 - 32768]", ("offset: " + describe(0x80128000, "LOAD R1, [R2 - 32768]")).c_str());
    check(disassemble(0x81210008) == "STORE R2, [R1 + 8]", ("offset: " + describe(0x81210008, "STORE R2, [R1 + 8]")).c_str());

    // jump targets: hex with 5 digits
    check(disassemble(0xC0000000) == "JMP 0x00000", ("target: " + describe(0xC0000000, "JMP 0x00000")).c_str());
    check(disassemble(0xC003FFFF) == "JMP 0x3FFFF", ("target: " + describe(0xC003FFFF, "JMP 0x3FFFF")).c_str());
    check(disassemble(0xC500001C) == "CALL 0x0001C", ("target: " + describe(0xC500001C, "CALL 0x0001C")).c_str());

    // unused fields are not shown
    check(disassemble(0x0E123456) == "HLT", ("unused fields: " + describe(0x0E123456, "HLT")).c_str());
    check(disassemble(0x0F910000) == "OUT R1", ("unused fields: " + describe(0x0F910000, "OUT R1")).c_str());
    check(disassemble(0xC2FC0004) == "JNZ 0x00004", ("unused fields: " + describe(0xC2FC0004, "JNZ 0x00004")).c_str());

    // unknown opcodes: no exception, full instruction in hex
    check(disassemble(0x10000000) == "??? 0x10000000", ("unknown: " + describe(0x10000000, "??? 0x10000000")).c_str());
    check(disassemble(0x4212ABCD) == "??? 0x4212ABCD", ("unknown: " + describe(0x4212ABCD, "??? 0x4212ABCD")).c_str());
    check(disassemble(0xFFFFFFFF) == "??? 0xFFFFFFFF", ("unknown: " + describe(0xFFFFFFFF, "??? 0xFFFFFFFF")).c_str());
    check(disassemble(0x00000000) == "MOV R0, R0", ("0x00000000 is a valid MOV: " + describe(0x00000000, "MOV R0, R0")).c_str());

    // round trip: assemble(disassemble(x)) == x for every example
    for (const Example& example : examples) {
        std::string text = disassemble(example.instruction);
        check(assembleLine(text) == example.instruction,
              ("round trip: " + describe(example.instruction, example.text) + " assembles back").c_str());
    }
}
