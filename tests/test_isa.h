// Written by Claude (AI); reviewed and run by me.
#pragma once

#include "TestSuite.h"

class IsaTests : public TestSuite {
    public:
        IsaTests() : TestSuite("Isa") {}

    protected:
        void runTests() override;
};