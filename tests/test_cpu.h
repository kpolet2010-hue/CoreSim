#pragma once

#include "TestSuite.h"

class CpuTests : public TestSuite {
    public:
        CpuTests() : TestSuite("CPU") {}

    protected:
        void runTests() override;
};
