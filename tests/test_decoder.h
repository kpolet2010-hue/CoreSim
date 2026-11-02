#pragma once

#include "TestSuite.h"

class DecoderTests : public TestSuite {
    public:
        DecoderTests() : TestSuite("Decoder") {}

    protected:
        void runTests() override;
};