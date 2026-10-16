#ifndef TEST_UTIL_HPP
#define TEST_UTIL_HPP

// Tiny test helpers. Unlike assert(), these still run in Release builds.
#include <cmath>
#include <iostream>
#include <stdexcept>

inline int g_checks = 0;
inline int g_failures = 0;

#define CHECK(cond)                                                          \
    do {                                                                     \
        ++g_checks;                                                          \
        if (!(cond)) {                                                       \
            ++g_failures;                                                    \
            std::cerr << __FILE__ << ":" << __LINE__                         \
                      << "  CHECK failed: " #cond "\n";                      \
        }                                                                    \
    } while (0)

#define CHECK_NEAR(a, b) CHECK(std::fabs((a) - (b)) < 1e-9)

// Passes only if the expression throws std::invalid_argument.
#define CHECK_THROWS(...)                                                    \
    do {                                                                     \
        ++g_checks;                                                          \
        bool threw = false;                                                  \
        try { __VA_ARGS__; } catch (const std::invalid_argument&) { threw = true; } \
        if (!threw) {                                                        \
            ++g_failures;                                                    \
            std::cerr << __FILE__ << ":" << __LINE__                         \
                      << "  expected std::invalid_argument from: " #__VA_ARGS__ "\n"; \
        }                                                                    \
    } while (0)

inline int reportResults(const char* suite)
{
    std::cout << suite << ": " << (g_checks - g_failures) << "/" << g_checks
              << " checks passed\n";
    return g_failures == 0 ? 0 : 1;
}

#endif  // TEST_UTIL_HPP