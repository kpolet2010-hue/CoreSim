#pragma once

#include <iostream>
#include <stdexcept>

// base class for tests
class TestSuite {
    public:
        explicit TestSuite(const char* name) : name_(name) {}
        virtual ~TestSuite() = default;

        // run all tests and returns number of failed checks
        int run() {
            runTests();
            std::cout << "[" << name_ << "] "
                    << (failures_ == 0 ? "passed" : "FAILED") << '\n';
            return failures_;
        }

    protected:
        virtual void runTests() = 0;

        void check(bool condition, const char* message) {
            if (condition) {
                std::cout << "  PASS [" << name_ << "]: " << message << '\n';
            } else {
                std::cerr << "  FAIL [" << name_ << "]: " << message << '\n';
                ++failures_;
            }
        }

        template <typename Func>
        static bool throwsOutOfRange(Func func) {
            try {
                func();
            } catch (const std::out_of_range&) {
                return true;
            }
            return false;
        }

    private:
        const char* name_;
        int failures_ = 0;
};
