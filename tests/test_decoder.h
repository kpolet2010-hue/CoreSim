// Written by Claude (AI); reviewed and run by me.
#pragma once

#include "TestSuite.h"

class DecoderTests : public TestSuite {
    public:
        DecoderTests() : TestSuite("Decoder") {}

    protected:
        void runTests() override;
};