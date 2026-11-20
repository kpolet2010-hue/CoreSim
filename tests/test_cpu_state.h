// Written by Claude (AI); reviewed and run by me.
#pragma once

#include "TestSuite.h"

class CpuStateTests : public TestSuite {
    public:
        CpuStateTests() : TestSuite("CPUState") {}

    protected:
        void runTests() override;
};