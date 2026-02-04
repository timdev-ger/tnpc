#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace doctest {

struct TestCase {
    const char* name;
    void (*func)();
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

struct Registrar {
    Registrar(const char* name, void (*func)()) {
        registry().push_back(TestCase{name, func});
    }
};

class FailureException : public std::exception {
public:
    const char* what() const noexcept override { return "doctest requirement failed"; }
};

inline int& failures() {
    static int count = 0;
    return count;
}

inline int& asserts() {
    static int count = 0;
    return count;
}

inline void check(bool condition, const char* expr, const char* file, int line, bool is_require) {
    asserts() += 1;
    if (!condition) {
        failures() += 1;
        std::cerr << file << ":" << line << " CHECK failed: " << expr << "\n";
        if (is_require) {
            throw FailureException{};
        }
    }
}

inline int run_all_tests() {
    int failed_tests = 0;
    for (const auto& test : registry()) {
        try {
            test.func();
        } catch (const FailureException&) {
            failed_tests += 1;
        }
    }
    if (failed_tests > 0) {
        std::cerr << "Tests failed: " << failed_tests << "\n";
    }
    return failed_tests;
}

}  // namespace doctest

#define DOCTEST_CAT_IMPL(x, y) x##y
#define DOCTEST_CAT(x, y) DOCTEST_CAT_IMPL(x, y)
#define DOCTEST_TEST_CASE_IMPL(name, id)                  \
    static void DOCTEST_CAT(test_, id)();                 \
    static doctest::Registrar DOCTEST_CAT(reg_, id)(name, &DOCTEST_CAT(test_, id)); \
    static void DOCTEST_CAT(test_, id)()
#define TEST_CASE(name) DOCTEST_TEST_CASE_IMPL(name, __COUNTER__)
#define CHECK(expr) doctest::check(static_cast<bool>(expr), #expr, __FILE__, __LINE__, false)
#define REQUIRE(expr) doctest::check(static_cast<bool>(expr), #expr, __FILE__, __LINE__, true)
