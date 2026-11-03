#pragma once

#include "TestSuite.h"

class RegisterTests : public TestSuite {
    public:
        RegisterTests() : TestSuite("Registers") {}

    protected:
        void runTests() override;
};
