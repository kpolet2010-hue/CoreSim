// Written by Claude (AI); reviewed and run by me.
#pragma once

#include "TestSuite.h"

class DisassemblerTests : public TestSuite {
    public:
        DisassemblerTests() : TestSuite("Disassembler") {}

    protected:
        void runTests() override;
};
