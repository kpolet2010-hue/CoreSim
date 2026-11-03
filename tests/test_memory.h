#pragma once

#include "TestSuite.h"

class MemoryTests : public TestSuite {
    public:
        MemoryTests() : TestSuite("Memory") {}

    protected:
        void runTests() override;
};
