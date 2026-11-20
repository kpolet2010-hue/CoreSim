// Written by Claude (AI); reviewed and run by me.
#pragma once

#include "TestSuite.h"

class CpuTests : public TestSuite {
    public:
        CpuTests() : TestSuite("CPU") {}

    protected:
        void runTests() override;
};
