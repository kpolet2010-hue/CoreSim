#pragma once

#include "TestSuite.h"

class CpuStateTests : public TestSuite {
    public:
        CpuStateTests() : TestSuite("CPUState") {}

    protected:
        void runTests() override;
};