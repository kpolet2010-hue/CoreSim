#include "test_assembler.h"

#include "coresim/Assembler.h"

#include <string>
#include <vector>

using namespace coresim;

namespace {

    bool operandsAre(const ParsedLine& parsed, const std::vector<std::string>& expected) {
        return parsed.operands == expected;
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
}
