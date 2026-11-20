// Written by Claude (AI); reviewed and run by me.
#pragma once

#include "TestSuite.h"

class LoaderTests : public TestSuite {
    public:
        LoaderTests() : TestSuite("Loader") {}

    protected:
        void runTests() override;
};
