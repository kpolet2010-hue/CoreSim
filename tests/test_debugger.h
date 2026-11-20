// Written by Claude (AI); reviewed and run by me.
#pragma once

#include "TestSuite.h"

class DebuggerTests : public TestSuite {
    public:
        DebuggerTests() : TestSuite("Debugger") {}

    protected:
        void runTests() override;
};
