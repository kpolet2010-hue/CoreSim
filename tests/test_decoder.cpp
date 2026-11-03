#include "test_decoder.h"

#include "coresim/DecodedInstruction.h"
#include "coresim/Decoder.h"

using namespace coresim;

void DecoderTests::runTests() {
    // prepare data
    Decoder decoder;

    // decode
    DecodedInstruction testCaseR = decoder.decode(0x12345000);
    DecodedInstruction testCaseI = decoder.decode(0x4123ABCD);
    DecodedInstruction testCaseM = decoder.decode(0x8012FFFC);
    DecodedInstruction testCaseM2 = decoder.decode(0x80120008);
    DecodedInstruction testCaseJ = decoder.decode(0xC0012345);
    DecodedInstruction testCaseJ2 = decoder.decode(0xFFFFFFFF);

    // test and check values
    check(testCaseR.opcode == 0x12, "Decoded Value: 0x12345000, Opcode: 0x12");
    check(testCaseR.rd == 3, "Decoded Value: 0x12345000, Rd: 3");
    check(testCaseR.rs1 == 4, "Decoded Value: 0x12345000, Rs1: 4");
    check(testCaseR.rs2 == 5, "Decoded Value: 0x12345000, Rs2: 5");

    check(testCaseI.opcode == 0x41, "Decoded Value: 0x4123ABCD, Opcode: 0x41");
    check(testCaseI.rd == 2, "Decoded Value: 0x4123ABCD, Rd: 2");
    check(testCaseI.rs1 == 3, "Decoded Value: 0x4123ABCD, Rs1: 3");
    check(testCaseI.immediate == 0xABCD, "Decoded Value: 0x4123ABCD, Immediate: 0xABCD");

    check(testCaseM.opcode == 0x80, "Decoded Value: 0x8012FFFC, Opcode: 0x80");
    check(testCaseM.rd == 1, "Decoded Value: 0x8012FFFC, Rd: 1");
    check(testCaseM.rs1 == 2, "Decoded Value: 0x8012FFFC, Rs1: 2");
    check(testCaseM.offset == -4, "Decoded Value: 0x8012FFFC, Offset: -4");

    check(testCaseM2.offset == 8, "Decoded Value: 0x80120008, Offset: 8");

    check(testCaseJ.opcode == 0xC0, "Decoded Value: 0xC0012345, Opcode: 0xC0");
    check(testCaseJ.address == 0x12345, "Decoded Value: 0xC0012345, Address: 0x12345");

    check(testCaseJ2.opcode == 0xFF, "Decoded Value: 0xFFFFFFFF, Opcode: 0xFF");
    check(testCaseJ2.address == 0x3FFFF, "Decoded Value: 0xFFFFFFFF, Address: 0x3FFFF");
    check(testCaseJ2.rd == 0, "Decoded Value: 0xFFFFFFFF, Rd: 0");
}