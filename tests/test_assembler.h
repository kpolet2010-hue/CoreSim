#pragma once

#include "TestSuite.h"

class AssemblerTests : public TestSuite {
    public:
        AssemblerTests() : TestSuite("Assembler") {}

    protected:
        void runTests() override;
};
