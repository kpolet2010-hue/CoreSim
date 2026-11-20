// Written by Claude (AI); reviewed and run by me.
#pragma once

#include "TestSuite.h"

class AluTests : public TestSuite {
    public:
        AluTests() : TestSuite("ALU") {}

    protected:
        void runTests() override;
};
