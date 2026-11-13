#pragma once

#include "TestSuite.h"

class DisassemblerTests : public TestSuite {
    public:
        DisassemblerTests() : TestSuite("Disassembler") {}

    protected:
        void runTests() override;
};
