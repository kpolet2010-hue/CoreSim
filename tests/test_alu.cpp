#include "test_alu.h"

#include "coresim/ALU.h"

using namespace coresim;

namespace {

    // compares the value and all four flags, flags are written in the order z, n, c, v
    bool matches(const AluResult& result, Word value, bool z, bool n, bool c, bool v) {
        return result.value == value
            && result.z == z
            && result.n == n
            && result.c == c
            && result.v == v;
    }

} // namespace

void AluTests::runTests() {
    const ALU alu;

    // ADD
    check(matches(alu.execute(AluOp::ADD, 5, 7), 12, 0, 0, 0, 0), "ADD: 5 + 7 = 12, no flags");
    check(matches(alu.execute(AluOp::ADD, 0, 0), 0, 1, 0, 0, 0), "ADD: 0 + 0 = 0 sets Z");
    check(matches(alu.execute(AluOp::ADD, 0xFFFF, 0), 0xFFFF, 0, 1, 0, 0), "ADD: 0xFFFF + 0 sets N, no carry");
    check(matches(alu.execute(AluOp::ADD, 0xFFFF, 1), 0, 1, 0, 1, 0), "ADD: 0xFFFF + 1 wraps to 0, sets Z and C");
    check(matches(alu.execute(AluOp::ADD, 0xFFFF, 0xFFFF), 0xFFFE, 0, 1, 1, 0), "ADD: 0xFFFF + 0xFFFF = 0xFFFE, sets N and C");
    check(matches(alu.execute(AluOp::ADD, 0x7FFF, 1), 0x8000, 0, 1, 0, 1), "ADD: 0x7FFF + 1 sets N and V");
    check(matches(alu.execute(AluOp::ADD, 0x8000, 0x8000), 0, 1, 0, 1, 1), "ADD: 0x8000 + 0x8000 sets Z, C and V");
    check(alu.execute(AluOp::ADD, 5, 7).writesResult, "ADD: result is written");

    // ADDI
    check(matches(alu.execute(AluOp::ADDI, 5, 7), 12, 0, 0, 0, 0), "ADDI: 5 + 7 = 12, no flags");
    check(matches(alu.execute(AluOp::ADDI, 1, 0xFFFF), 0, 1, 0, 1, 0), "ADDI: 1 + 0xFFFF wraps to 0, sets Z and C");
    check(matches(alu.execute(AluOp::ADDI, 0x7FFF, 1), 0x8000, 0, 1, 0, 1), "ADDI: 0x7FFF + 1 sets N and V");
    check(alu.execute(AluOp::ADDI, 5, 7).writesResult, "ADDI: result is written");

    // SUB
    check(matches(alu.execute(AluOp::SUB, 7, 5), 2, 0, 0, 0, 0), "SUB: 7 - 5 = 2, no flags");
    check(matches(alu.execute(AluOp::SUB, 5, 5), 0, 1, 0, 0, 0), "SUB: 5 - 5 = 0 sets Z");
    check(matches(alu.execute(AluOp::SUB, 0xFFFF, 0xFFFF), 0, 1, 0, 0, 0), "SUB: 0xFFFF - 0xFFFF = 0 sets Z");
    check(matches(alu.execute(AluOp::SUB, 0xFFFF, 1), 0xFFFE, 0, 1, 0, 0), "SUB: 0xFFFF - 1 sets N, no borrow");
    check(matches(alu.execute(AluOp::SUB, 0, 1), 0xFFFF, 0, 1, 1, 0), "SUB: 0 - 1 wraps to 0xFFFF, sets N and C");
    check(matches(alu.execute(AluOp::SUB, 0x8000, 1), 0x7FFF, 0, 0, 0, 1), "SUB: 0x8000 - 1 sets V");
    check(matches(alu.execute(AluOp::SUB, 0x7FFF, 0xFFFF), 0x8000, 0, 1, 1, 1), "SUB: 0x7FFF - 0xFFFF sets N, C and V");
    check(alu.execute(AluOp::SUB, 7, 5).writesResult, "SUB: result is written");

    // MUL
    check(matches(alu.execute(AluOp::MUL, 3, 4), 12, 0, 0, 0, 0), "MUL: 3 * 4 = 12, no flags");
    check(matches(alu.execute(AluOp::MUL, 0x1234, 0), 0, 1, 0, 0, 0), "MUL: 0x1234 * 0 = 0 sets Z");
    check(matches(alu.execute(AluOp::MUL, 0x00FF, 0x0101), 0xFFFF, 0, 1, 0, 0), "MUL: 0xFF * 0x101 = 0xFFFF fits, no carry");
    check(matches(alu.execute(AluOp::MUL, 0x0100, 0x0100), 0, 1, 0, 1, 0), "MUL: 0x100 * 0x100 keeps lower bits 0, sets Z and C");
    check(matches(alu.execute(AluOp::MUL, 0x1234, 0x0010), 0x2340, 0, 0, 1, 0), "MUL: 0x1234 * 0x10 keeps lower 16 bits, sets C");
    check(matches(alu.execute(AluOp::MUL, 0xFFFF, 0xFFFF), 1, 0, 0, 1, 0), "MUL: 0xFFFF * 0xFFFF keeps lower 16 bits, sets C");
    check(matches(alu.execute(AluOp::MUL, 0x4000, 2), 0x8000, 0, 1, 0, 0), "MUL: 0x4000 * 2 sets N, V stays 0");
    check(alu.execute(AluOp::MUL, 3, 4).writesResult, "MUL: result is written");

    // DIV
    check(matches(alu.execute(AluOp::DIV, 12, 4), 3, 0, 0, 0, 0), "DIV: 12 / 4 = 3, no flags");
    check(matches(alu.execute(AluOp::DIV, 7, 2), 3, 0, 0, 0, 0), "DIV: 7 / 2 rounds down to 3");
    check(matches(alu.execute(AluOp::DIV, 0, 5), 0, 1, 0, 0, 0), "DIV: 0 / 5 = 0 sets Z");
    check(matches(alu.execute(AluOp::DIV, 1, 2), 0, 1, 0, 0, 0), "DIV: 1 / 2 = 0 sets Z");
    check(matches(alu.execute(AluOp::DIV, 0xFFFF, 1), 0xFFFF, 0, 1, 0, 0), "DIV: 0xFFFF / 1 sets N");
    check(matches(alu.execute(AluOp::DIV, 0xFFFF, 2), 0x7FFF, 0, 0, 0, 0), "DIV: 0xFFFF / 2 = 0x7FFF (unsigned)");
    check(matches(alu.execute(AluOp::DIV, 0xFFFF, 0xFFFF), 1, 0, 0, 0, 0), "DIV: 0xFFFF / 0xFFFF = 1");
    check(matches(alu.execute(AluOp::DIV, 0x8000, 0xFFFF), 0, 1, 0, 0, 0), "DIV: 0x8000 / 0xFFFF = 0 (unsigned)");
    check(alu.execute(AluOp::DIV, 12, 4).writesResult, "DIV: result is written");

    check(throwsRuntimeError([&] { alu.execute(AluOp::DIV, 5, 0); }), "DIV: 5 / 0 throws runtime_error");
    check(throwsRuntimeError([&] { alu.execute(AluOp::DIV, 0, 0); }), "DIV: 0 / 0 throws runtime_error");

    // CMP calculates like SUB, but has no result to store
    check(matches(alu.execute(AluOp::CMP, 5, 5), 0, 1, 0, 0, 0), "CMP: 5, 5 sets Z");
    check(matches(alu.execute(AluOp::CMP, 5, 3), 2, 0, 0, 0, 0), "CMP: 5, 3 sets no flags");
    check(matches(alu.execute(AluOp::CMP, 3, 5), 0xFFFE, 0, 1, 1, 0), "CMP: 3, 5 sets N and C");
    check(matches(alu.execute(AluOp::CMP, 0x8000, 1), 0x7FFF, 0, 0, 0, 1), "CMP: 0x8000, 1 sets V");
    check(matches(alu.execute(AluOp::CMP, 0xFFFF, 0xFFFF), 0, 1, 0, 0, 0), "CMP: 0xFFFF, 0xFFFF sets Z");
    check(!alu.execute(AluOp::CMP, 5, 3).writesResult, "CMP: result is not written");

    // AND
    check(matches(alu.execute(AluOp::AND, 0x00FF, 0x0F0F), 0x000F, 0, 0, 0, 0), "AND: 0x00FF & 0x0F0F = 0x000F, no flags");
    check(matches(alu.execute(AluOp::AND, 0x0F0F, 0xF0F0), 0, 1, 0, 0, 0), "AND: 0x0F0F & 0xF0F0 = 0 sets Z");
    check(matches(alu.execute(AluOp::AND, 0xFFFF, 0xFFFF), 0xFFFF, 0, 1, 0, 0), "AND: 0xFFFF & 0xFFFF sets N");
    check(alu.execute(AluOp::AND, 1, 1).writesResult, "AND: result is written");

    // OR
    check(matches(alu.execute(AluOp::OR, 0x00F0, 0x0F00), 0x0FF0, 0, 0, 0, 0), "OR: 0x00F0 | 0x0F00 = 0x0FF0, no flags");
    check(matches(alu.execute(AluOp::OR, 0, 0), 0, 1, 0, 0, 0), "OR: 0 | 0 = 0 sets Z");
    check(matches(alu.execute(AluOp::OR, 0x8000, 1), 0x8001, 0, 1, 0, 0), "OR: 0x8000 | 1 sets N");
    check(matches(alu.execute(AluOp::OR, 0xFFFF, 0), 0xFFFF, 0, 1, 0, 0), "OR: 0xFFFF | 0 sets N");
    check(alu.execute(AluOp::OR, 1, 1).writesResult, "OR: result is written");

    // XOR
    check(matches(alu.execute(AluOp::XOR, 0x0F00, 0x00F0), 0x0FF0, 0, 0, 0, 0), "XOR: 0x0F00 ^ 0x00F0 = 0x0FF0, no flags");
    check(matches(alu.execute(AluOp::XOR, 0xABCD, 0xABCD), 0, 1, 0, 0, 0), "XOR: equal operands give 0, sets Z");
    check(matches(alu.execute(AluOp::XOR, 0xFFFF, 1), 0xFFFE, 0, 1, 0, 0), "XOR: 0xFFFF ^ 1 sets N");
    check(alu.execute(AluOp::XOR, 1, 2).writesResult, "XOR: result is written");

    // NOT
    check(matches(alu.execute(AluOp::NOT, 0xF0F0), 0x0F0F, 0, 0, 0, 0), "NOT: ~0xF0F0 = 0x0F0F, no flags");
    check(matches(alu.execute(AluOp::NOT, 0xFFFF), 0, 1, 0, 0, 0), "NOT: ~0xFFFF = 0 sets Z");
    check(matches(alu.execute(AluOp::NOT, 0), 0xFFFF, 0, 1, 0, 0), "NOT: ~0 = 0xFFFF sets N");
    check(matches(alu.execute(AluOp::NOT, 0, 0x1234), 0xFFFF, 0, 1, 0, 0), "NOT: second operand is ignored");
    check(alu.execute(AluOp::NOT, 0).writesResult, "NOT: result is written");
}
