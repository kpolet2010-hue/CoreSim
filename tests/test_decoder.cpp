#include "test_decoder.h"

#include "coresim/DecodedInstruction.h"
#include "coresim/Decoder.h"

using namespace coresim;

void DecoderTests::runTests() {
    // prepare data
    Decoder decoder;

    // decode
    DecodedInstruction testCase1 = decoder.decode(0x12345000);
    DecodedInstruction testCase2 = decoder.decode(0xABCD0000);
    DecodedInstruction testCase3 = decoder.decode(0xFFFFFFFF);
    DecodedInstruction testCase4 = decoder.decode(0x00000000);
    DecodedInstruction testCase5 = decoder.decode(0xF0123000);

    // test and check values
    check(testCase1.opcode == 0x12, "Test 1: 0x12");
    check(testCase1.rd == 3, "Test 2: 3");
    check(testCase1.rs1 == 4, "Test 3: 4");
    check(testCase1.rs2 == 5, "Test 4: 5");

    check(testCase2.opcode == 0xAB, "Test 1: 0xAB");
    check(testCase2.rd == 12, "Test 2: 12");
    check(testCase2.rs1 == 13, "Test 3: 13");
    check(testCase2.rs2 == 0, "Test 4: 0");
    
    check(testCase3.opcode == 0xFF, "Test 1: 0xFF");
    check(testCase3.rd == 15, "Test 2: 15");
    check(testCase3.rs1 == 15, "Test 3: 15");
    check(testCase3.rs2 == 15, "Test 4: 15");

    check(testCase4.opcode == 0x00, "Test 1: 0x00");
    check(testCase4.rd == 0, "Test 2: 0");
    check(testCase4.rs1 == 0, "Test 3: 0");
    check(testCase4.rs2 == 0, "Test 4: 0");

    check(testCase5.opcode == 0xF0, "Test 1: 0xF0");
    check(testCase5.rd == 1, "Test 2: 1");
    check(testCase5.rs1 == 2, "Test 3: 2");
    check(testCase5.rs2 == 3, "Test 4: 3");
}