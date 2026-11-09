#pragma once

#include "TestSuite.h"

class LoaderTests : public TestSuite {
    public:
        LoaderTests() : TestSuite("Loader") {}

    protected:
        void runTests() override;
};
