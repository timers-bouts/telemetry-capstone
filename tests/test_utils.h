#pragma once

// Minimal inline test framework — no external dependencies.
// Usage:
//   test_pass("round-trip u32");
//   test_fail("CRC mismatch", "expected 0xDEADBEEF, got 0x00000000");

#include <iostream>
#include <string>

inline int g_tests_run    = 0;
inline int g_tests_passed = 0;
inline int g_tests_failed = 0;

inline void test_pass(const std::string& name) {
    ++g_tests_run;
    ++g_tests_passed;
    std::cout << "  [PASS] " << name << "\n";
}

inline void test_fail(const std::string& name, const std::string& reason = "") {
    ++g_tests_run;
    ++g_tests_failed;
    std::cout << "  [FAIL] " << name;
    if (!reason.empty()) std::cout << " — " << reason;
    std::cout << "\n";
}

// Call at end of main() to print summary and return appropriate exit code.
inline int test_summary() {
    std::cout << "\n" << g_tests_passed << "/" << g_tests_run << " tests passed";
    if (g_tests_failed > 0) std::cout << " (" << g_tests_failed << " failed)";
    std::cout << "\n";
    return g_tests_failed == 0 ? 0 : 1;
}