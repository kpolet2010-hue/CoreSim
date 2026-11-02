#pragma once

#include <iostream>
#include <stdexcept>

// Base class for a group of related tests.
// Derived classes implement runTests() and use check() for assertions.
class TestSuite {
public:
    explicit TestSuite(const char* name) : name_(name) {}
    virtual ~TestSuite() = default;

    // Runs all tests and returns the number of failed checks.
    int run() {
        runTests();
        std::cout << "[" << name_ << "] "
                  << (failures_ == 0 ? "passed" : "FAILED") << '\n';
        return failures_;
    }

protected:
    virtual void runTests() = 0;

    void check(bool condition, const char* message) {
        if (!condition) {
            std::cerr << "FAIL [" << name_ << "]: " << message << '\n';
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
