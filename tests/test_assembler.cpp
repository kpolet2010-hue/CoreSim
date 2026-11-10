#include "test_assembler.h"

#include "coresim/Assembler.h"
#include "coresim/Memory.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace coresim;

namespace {

    bool operandsAre(const ParsedLine& parsed, const std::vector<std::string>& expected) {
        return parsed.operands == expected;
    }

    std::vector<Instruction> assembleText(const std::string& source) {
        std::istringstream input(source);
        return assemble(input);
    }

    // assembles a program of exactly one instruction and compares it
    bool encodesTo(const std::string& line, Instruction expected) {
        std::vector<Instruction> program = assembleText(line);
        return program.size() == 1 && program[0] == expected;
    }

    // true if assembling throws std::runtime_error with exactly this message
    bool failsWith(const std::string& source, const std::string& message) {
        try {
            assembleText(source);
        } catch (const std::runtime_error& error) {
            return error.what() == message;
        }
        return false;
    }

} // namespace

void AssemblerTests::runTests() {
    // parseLine: empty lines and comments
    ParsedLine empty = parseLine("");
    check(empty.label.empty() && empty.mnemonic.empty() && empty.operands.empty(),
          "parseLine: empty line has no parts");

    ParsedLine blank = parseLine("   \t  ");
    check(blank.label.empty() && blank.mnemonic.empty() && blank.operands.empty(),
          "parseLine: whitespace-only line has no parts");

    ParsedLine comment = parseLine("; only a comment");
    check(comment.label.empty() && comment.mnemonic.empty() && comment.operands.empty(),
          "parseLine: comment-only line has no parts");

    // parseLine: instruction without operands
    ParsedLine hlt = parseLine("HLT");
    check(hlt.label.empty(), "parseLine: HLT has no label");
    check(hlt.mnemonic == "HLT", "parseLine: HLT mnemonic");
    check(hlt.operands.empty(), "parseLine: HLT has no operands");

    ParsedLine hltComment = parseLine("HLT ; stop");
    check(hltComment.mnemonic == "HLT", "parseLine: HLT before a comment");
    check(hltComment.operands.empty(), "parseLine: spaces before a comment are no operand");

    ParsedLine hltTrailing = parseLine("HLT \t ");
    check(hltTrailing.mnemonic == "HLT" && hltTrailing.operands.empty(),
          "parseLine: trailing spaces and tabs are no operand");

    // parseLine: operands, separated by commas
    ParsedLine addi = parseLine("ADDI R1, R1, -1");
    check(addi.mnemonic == "ADDI", "parseLine: ADDI mnemonic");
    check(operandsAre(addi, {"R1", "R1", "-1"}), "parseLine: ADDI has 3 operands");

    ParsedLine out = parseLine("OUT R1");
    check(out.mnemonic == "OUT", "parseLine: OUT mnemonic");
    check(operandsAre(out, {"R1"}), "parseLine: OUT has 1 operand");

    // parseLine: comment after an instruction is cut off
    ParsedLine withComment = parseLine("ADDI R1, R1, -1 ; count down");
    check(withComment.mnemonic == "ADDI", "parseLine: mnemonic before a comment");
    check(operandsAre(withComment, {"R1", "R1", "-1"}), "parseLine: comment is not part of the last operand");

    // parseLine: spaces and tabs are trimmed
    ParsedLine spaced = parseLine("  \tADD   R4 ,R1 ,  R2\t ");
    check(spaced.mnemonic == "ADD", "parseLine: mnemonic with leading spaces and tabs");
    check(operandsAre(spaced, {"R4", "R1", "R2"}), "parseLine: spaces around commas are trimmed");

    ParsedLine tabbed = parseLine("OUT\tR1");
    check(tabbed.mnemonic == "OUT", "parseLine: tab between mnemonic and operand");
    check(operandsAre(tabbed, {"R1"}), "parseLine: operand after a tab");

    // parseLine: labels
    ParsedLine loop = parseLine("loop:   OUT  R1");
    check(loop.label == "loop", "parseLine: label before an instruction, without ':'");
    check(loop.mnemonic == "OUT", "parseLine: mnemonic after a label");
    check(operandsAre(loop, {"R1"}), "parseLine: operand after a label");

    ParsedLine labelOnly = parseLine("end:");
    check(labelOnly.label == "end", "parseLine: label alone in a line");
    check(labelOnly.mnemonic.empty() && labelOnly.operands.empty(), "parseLine: label alone has no instruction");

    ParsedLine labelComment = parseLine("end:   ; the end");
    check(labelComment.label == "end", "parseLine: label followed by a comment");
    check(labelComment.mnemonic.empty(), "parseLine: label with comment has no instruction");

    ParsedLine labelNoSpace = parseLine("loop:OUT R1");
    check(labelNoSpace.label == "loop", "parseLine: label without space after ':'");
    check(labelNoSpace.mnemonic == "OUT", "parseLine: mnemonic directly after ':'");

    // parseLine: memory operand stays one operand
    ParsedLine load = parseLine("LOAD R3, [R1 + 8]");
    check(load.mnemonic == "LOAD", "parseLine: LOAD mnemonic");
    check(operandsAre(load, {"R3", "[R1 + 8]"}), "parseLine: [R1 + 8] stays one operand");

    // parseLine: letter case is kept, checking it is done later
    ParsedLine lower = parseLine("movi r1, 0x1C");
    check(lower.mnemonic == "movi", "parseLine: letter case of the mnemonic is kept");
    check(operandsAre(lower, {"r1", "0x1C"}), "parseLine: letter case of the operands is kept");

    // parseLine: spaces around a label are removed
    ParsedLine indented = parseLine("  \tloop :  OUT R1");
    check(indented.label == "loop", "parseLine: spaces and tabs around a label are trimmed");

    // assemble: encoding of every instruction (examples from ASSEMBLER.md where possible)
    check(encodesTo("MOV R2, R1", 0x00210000), "assemble: MOV R2, R1 = 00210000");
    check(encodesTo("MOVI R1, 3", 0x40100003), "assemble: MOVI R1, 3 = 40100003");
    check(encodesTo("ADD R4, R1, R2", 0x01412000), "assemble: ADD R4, R1, R2 = 01412000");
    check(encodesTo("SUB R3, R1, R2", 0x02312000), "assemble: SUB R3, R1, R2 = 02312000");
    check(encodesTo("MUL R3, R1, R2", 0x03312000), "assemble: MUL R3, R1, R2 = 03312000");
    check(encodesTo("DIV R3, R1, R2", 0x04312000), "assemble: DIV R3, R1, R2 = 04312000");
    check(encodesTo("ADDI R1, R1, -1", 0x4111FFFF), "assemble: ADDI R1, R1, -1 = 4111FFFF");
    check(encodesTo("AND R3, R1, R2", 0x05312000), "assemble: AND R3, R1, R2 = 05312000");
    check(encodesTo("OR R3, R1, R2", 0x06312000), "assemble: OR R3, R1, R2 = 06312000");
    check(encodesTo("XOR R3, R1, R2", 0x07312000), "assemble: XOR R3, R1, R2 = 07312000");
    check(encodesTo("NOT R3, R1", 0x08310000), "assemble: NOT R3, R1 = 08310000");
    check(encodesTo("CMP R1, R2", 0x09012000), "assemble: CMP R1, R2 = 09012000 (rd field 0)");
    check(encodesTo("PUSH R1", 0x0A010000), "assemble: PUSH R1 = 0A010000 (rs1)");
    check(encodesTo("POP R2", 0x0B200000), "assemble: POP R2 = 0B200000 (rd)");
    check(encodesTo("RET", 0x0C000000), "assemble: RET = 0C000000");
    check(encodesTo("CALL 0x00100", 0xC5000100), "assemble: CALL 0x00100 = C5000100");
    check(encodesTo("NOP", 0x0D000000), "assemble: NOP = 0D000000");
    check(encodesTo("HLT", 0x0E000000), "assemble: HLT = 0E000000");
    check(encodesTo("OUT R1", 0x0F010000), "assemble: OUT R1 = 0F010000 (rs1)");
    check(encodesTo("LOAD R3, [R1 + 8]", 0x80310008), "assemble: LOAD R3, [R1 + 8] = 80310008");
    check(encodesTo("STORE R2, [R1 + 8]", 0x81210008), "assemble: STORE R2, [R1 + 8] = 81210008 (rs in rd field)");
    check(encodesTo("JMP 0x00100", 0xC0000100), "assemble: JMP 0x00100 = C0000100");
    check(encodesTo("JZ 0x00100", 0xC1000100), "assemble: JZ 0x00100 = C1000100");
    check(encodesTo("JNZ 0x00100", 0xC2000100), "assemble: JNZ 0x00100 = C2000100");
    check(encodesTo("JN 0x00100", 0xC3000100), "assemble: JN 0x00100 = C3000100");
    check(encodesTo("JC 0x00100", 0xC4000100), "assemble: JC 0x00100 = C4000100");

    // assemble: registers
    check(encodesTo("ADD R15, R14, R13", 0x01FED000), "assemble: R13 to R15 use the whole 4-bit field");
    check(encodesTo("ADD R0, R0, R0", 0x01000000), "assemble: R0 gives 0 in every field");
    check(encodesTo("SUB R1, R1, R1", 0x02111000), "assemble: the same register in rd, rs1 and rs2");

    // assemble: numbers
    check(encodesTo("MOVI R1, 0", 0x40100000), "assemble: MOVI 0");
    check(encodesTo("MOVI R1, 42", 0x4010002A), "assemble: decimal 42 = 0x002A");
    check(encodesTo("MOVI R1, 0x1C", 0x4010001C), "assemble: hex 0x1C");
    check(encodesTo("MOVI R1, 0X1c", 0x4010001C), "assemble: hex with 0X and lower case digits");
    check(encodesTo("MOVI R1, 65535", 0x4010FFFF), "assemble: MOVI 65535 = 0xFFFF (largest unsigned)");
    check(encodesTo("MOVI R1, 0xFFFF", 0x4010FFFF), "assemble: MOVI 0xFFFF");
    check(encodesTo("MOVI R1, -1", 0x4010FFFF), "assemble: MOVI -1 = 0xFFFF (two's complement)");
    check(encodesTo("MOVI R1, -32768", 0x40108000), "assemble: MOVI -32768 = 0x8000 (smallest signed)");

    // assemble: memory operands
    check(encodesTo("LOAD R3, [R1]", 0x80310000), "assemble: [R1] has offset 0");
    check(encodesTo("LOAD R3, [R1 - 4]", 0x8031FFFC), "assemble: [R1 - 4] = offset 0xFFFC");
    check(encodesTo("LOAD R3, [R1+0x20]", 0x80310020), "assemble: memory operand without spaces, hex offset");
    check(encodesTo("LOAD R1, [R2 + 32767]", 0x80127FFF), "assemble: largest offset 32767");
    check(encodesTo("LOAD R1, [R2 - 32768]", 0x80128000), "assemble: smallest offset -32768");
    check(encodesTo("STORE R1, [R1 + 2]", 0x81110002), "assemble: STORE with value register equal to base register");

    // assemble: jump targets as numbers
    check(encodesTo("JMP 0", 0xC0000000), "assemble: JMP 0");
    check(encodesTo("JMP 0x3FFFF", 0xC003FFFF), "assemble: JMP 0x3FFFF (largest address)");
    check(encodesTo("CALL 16", 0xC5000010), "assemble: decimal jump target 16 = 0x10");

    // assemble: letter case of mnemonics and registers is ignored
    check(encodesTo("movi r1, 0x1c", 0x4010001C), "assemble: lower case mnemonic and register");
    check(encodesTo("Add R4, r1, R2", 0x01412000), "assemble: mixed letter case");

    // assemble: whole programs, comments and empty lines
    check(assembleText("").empty(), "assemble: empty source gives no instructions");
    check(assembleText("; only comments\n\n   \n").empty(), "assemble: comments and empty lines give no instructions");
    check(assembleText("MOVI R1, 3\r\nOUT R1\r\nHLT\r\n")
              == std::vector<Instruction>{0x40100003, 0x0F010000, 0x0E000000},
          "assemble: Windows line endings");

    // assemble: the countdown from ASSEMBLER.md gives exactly examples/countdown.hex
    check(assembleText("; countdown.asm - counts down from 3 to 1\n"
                       "        MOVI R1, 3\n"
                       "loop:   OUT  R1\n"
                       "        ADDI R1, R1, -1\n"
                       "        JNZ  loop\n"
                       "        HLT\n")
              == std::vector<Instruction>{0x40100003, 0x0F010000, 0x4111FFFF, 0xC2000004, 0x0E000000},
          "assemble: countdown from ASSEMBLER.md");

    // assemble: labels
    check(assembleText("JMP end\nNOP\nend: HLT")
              == std::vector<Instruction>{0xC0000008, 0x0D000000, 0x0E000000},
          "assemble: jump to a label defined further down");
    check(assembleText("start: NOP\nJMP start")
              == std::vector<Instruction>{0x0D000000, 0xC0000000},
          "assemble: jump back to a label at address 0");
    check(assembleText("JMP end\nend:\n; comment\n\nHLT")
              == std::vector<Instruction>{0xC0000004, 0x0E000000},
          "assemble: a label alone marks the next instruction, comments and empty lines are skipped");
    check(assembleText("JMP end\nend:")
              == std::vector<Instruction>{0xC0000004},
          "assemble: a label after the last instruction marks the address after it");
    check(assembleText("loop: NOP\nLoop: NOP\nJMP Loop")
              == std::vector<Instruction>{0x0D000000, 0x0D000000, 0xC0000004},
          "assemble: letter case of labels matters");
    check(assembleText("_x1: NOP\nCALL _x1")
              == std::vector<Instruction>{0x0D000000, 0xC5000000},
          "assemble: label with '_' and digits");
    check(assembleText("NOP\n  \tsquare:  RET\nCALL square")
              == std::vector<Instruction>{0x0D000000, 0x0C000000, 0xC5000004},
          "assemble: indented label");
    check(assembleText("R16: NOP\nJMP R16")
              == std::vector<Instruction>{0x0D000000, 0xC0000000},
          "assemble: R16 is no register, so it can be a label");

    // assemble: errors, the line number counts comments and empty lines
    check(failsWith("MOVE R1, 3", "line 1: unknown instruction 'MOVE'"), "error: unknown mnemonic");
    check(failsWith("; comment\n\nADD R1, R2", "line 3: 'ADD' expects 3 operands, got 2"),
          "error: too few operands, line number counts comments and empty lines");
    check(failsWith("ADD R1, R2, R3, R4", "line 1: 'ADD' expects 3 operands, got 4"), "error: too many operands");
    check(failsWith("HLT R1", "line 1: 'HLT' expects 0 operands, got 1"), "error: operand for HLT");
    check(failsWith("OUT", "line 1: 'OUT' expects 1 operand, got 0"), "error: missing operand, singular 'operand'");
    check(failsWith("ADD R1, R2, 3", "line 1: expected a register, got '3'"), "error: number instead of register");
    check(failsWith("ADD R1, , R2", "line 1: expected a register, got ''"), "error: empty operand");
    check(failsWith("OUT R16", "line 1: invalid register 'R16'"), "error: R16 does not exist");
    check(failsWith("OUT RX", "line 1: invalid register 'RX'"), "error: RX is no register");
    check(failsWith("OUT R01", "line 1: invalid register 'R01'"), "error: register with leading zero");
    check(failsWith("MOVI R1, 70000", "line 1: number out of range '70000'"), "error: immediate above 65535");
    check(failsWith("MOVI R1, 65536", "line 1: number out of range '65536'"), "error: immediate 65536");
    check(failsWith("MOVI R1, -32769", "line 1: number out of range '-32769'"), "error: immediate below -32768");
    check(failsWith("MOVI R1, 123456789012", "line 1: number out of range '123456789012'"), "error: very long number");
    check(failsWith("MOVI R1, abc", "line 1: expected a number, got 'abc'"), "error: text instead of number");
    check(failsWith("MOVI R1, R2", "line 1: expected a number, got 'R2'"), "error: register instead of number");
    check(failsWith("MOVI R1, 0x", "line 1: expected a number, got '0x'"), "error: 0x without digits");
    check(failsWith("MOVI R1, 0x1G", "line 1: expected a number, got '0x1G'"), "error: invalid hex digit");
    check(failsWith("LOAD R1, [R2 + 40000]", "line 1: offset out of range '[R2 + 40000]'"), "error: offset 40000");
    check(failsWith("LOAD R1, [R2 + 32768]", "line 1: offset out of range '[R2 + 32768]'"), "error: offset 32768");
    check(failsWith("LOAD R1, R2 + 4", "line 1: malformed memory operand 'R2 + 4'"), "error: brackets missing");
    check(failsWith("LOAD R1, [R2 + 4", "line 1: malformed memory operand '[R2 + 4'"), "error: closing bracket missing");
    check(failsWith("LOAD R1, [R2 +]", "line 1: malformed memory operand '[R2 +]'"), "error: offset missing after '+'");
    check(failsWith("LOAD R1, [R16]", "line 1: invalid register 'R16'"), "error: invalid base register");
    check(failsWith("JMP 0x40000", "line 1: number out of range '0x40000'"), "error: jump target above 0x3FFFF");
    check(failsWith("JMP -4", "line 1: number out of range '-4'"), "error: negative jump target");
    check(failsWith("loop: NOP\nJNZ lop", "line 2: unknown label 'lop'"), "error: unknown label");
    check(failsWith("loop: NOP\nloop: HLT", "line 2: label 'loop' defined twice"), "error: label defined twice");
    check(failsWith("1loop: HLT", "line 1: invalid label name '1loop'"), "error: label starts with a digit");
    check(failsWith("R1: HLT", "line 1: invalid label name 'R1'"), "error: register name as label");
    check(failsWith("r1: HLT", "line 1: invalid label name 'r1'"), "error: lower case register name as label");
    check(failsWith("add: HLT", "line 1: invalid label name 'add'"), "error: mnemonic as label");
    check(failsWith("my-loop: HLT", "line 1: invalid label name 'my-loop'"), "error: '-' in a label");
    check(failsWith(": HLT", "line 1: invalid label name ''"), "error: empty label");
    check(assembleText("HLT ; note: done").size() == 1, "assemble: ':' inside a comment is no label");

    // assemble: the program has to fit into memory (65,536 instructions)
    std::string full;
    for (int i = 0; i < 65536; ++i) {
        full += "NOP\n";
    }
    check(assembleText(full).size() == 65536, "assemble: 65,536 instructions fit into memory");
    check(failsWith(full + "HLT", "line 65537: program does not fit into memory"), "error: one more instruction does not fit");

    // loadAsm writes the program little-endian from address 0 and returns the count
    Memory memory;
    std::istringstream countdown("MOVI R1, 3\nloop: OUT R1\nADDI R1, R1, -1\nJNZ loop\nHLT\n");
    check(loadAsm(countdown, memory) == 5, "loadAsm: returns 5 instructions");
    check(memory.read32(0x00) == 0x40100003, "loadAsm: first instruction at 0x00000");
    check(memory.read32(0x10) == 0x0E000000, "loadAsm: fifth instruction at 0x00010");
    check(memory.read8(0x00) == 0x03 && memory.read8(0x03) == 0x40, "loadAsm: little-endian");

    // loadAsm writes nothing if a line has an error, also not the lines before it
    Memory untouched;
    untouched.write32(0x00, 0x12345678);
    std::istringstream broken("NOP\nNOP\nMOVE R1, 3\n");
    check(throwsRuntimeError([&] { loadAsm(broken, untouched); }), "loadAsm: an error throws runtime_error");
    check(untouched.read32(0x00) == 0x12345678, "loadAsm: memory is unchanged after an error");
}
