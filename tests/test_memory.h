// Written by Claude (AI); reviewed and run by me.
#pragma once

#include "TestSuite.h"

class MemoryTests : public TestSuite {
    public:
        MemoryTests() : TestSuite("Memory") {}

    protected:
        void runTests() override;
};
