// Written by Claude (AI); reviewed and run by me.
#pragma once

#include "TestSuite.h"

class ExamplesTests : public TestSuite {
    public:
        ExamplesTests() : TestSuite("Examples") {}

    protected:
        void runTests() override;
};
