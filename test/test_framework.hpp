#pragma once
// ModernUI — Minimal header-only test framework
#include <cstdio>
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <stdexcept>

namespace test {

struct TestCase {
    const char* name;
    std::function<void()> fn;
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> r;
    return r;
}

struct AutoRegister {
    AutoRegister(const char* name, std::function<void()> fn) {
        registry().push_back({name, std::move(fn)});
    }
};

#define TEST(name) \
    static void test_##name(); \
    static ::test::AutoRegister _reg_##name(#name, test_##name); \
    static void test_##name()

// Assertion helpers
inline int g_assertions = 0;
inline int g_failures = 0;
inline const char* g_current_test = nullptr;

inline void fail(const char* file, int line, const char* msg) {
    fprintf(stderr, "  FAIL [%s:%d]: %s\n", file, line, msg);
    g_failures++;
    throw std::runtime_error("test assertion failed");
}

#define ASSERT_TRUE(cond) do { \
    ::test::g_assertions++; \
    if (!(cond)) ::test::fail(__FILE__, __LINE__, #cond " is false"); \
} while(0)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(a, b) do { \
    ::test::g_assertions++; \
    if (!((a) == (b))) { \
        char _buf[256]; snprintf(_buf, sizeof(_buf), #a " == " #b " failed"); \
        ::test::fail(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_NE(a, b) do { \
    ::test::g_assertions++; \
    if ((a) == (b)) ::test::fail(__FILE__, __LINE__, #a " != " #b " failed"); \
} while(0)

#define ASSERT_FLOAT_EQ(a, b, eps) do { \
    ::test::g_assertions++; \
    if (std::fabs((a) - (b)) > (eps)) { \
        char _buf[256]; snprintf(_buf, sizeof(_buf), #a " ≈ " #b " failed (%f vs %f)", (double)(a), (double)(b)); \
        ::test::fail(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_GT(a, b) do { \
    ::test::g_assertions++; \
    if (!((a) > (b))) { \
        char _buf[256]; snprintf(_buf, sizeof(_buf), #a " > " #b " failed"); \
        ::test::fail(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_LT(a, b) do { \
    ::test::g_assertions++; \
    if (!((a) < (b))) { \
        char _buf[256]; snprintf(_buf, sizeof(_buf), #a " < " #b " failed"); \
        ::test::fail(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_GE(a, b) do { \
    ::test::g_assertions++; \
    if (!((a) >= (b))) { \
        char _buf[256]; snprintf(_buf, sizeof(_buf), #a " >= " #b " failed"); \
        ::test::fail(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_LE(a, b) do { \
    ::test::g_assertions++; \
    if (!((a) <= (b))) { \
        char _buf[256]; snprintf(_buf, sizeof(_buf), #a " <= " #b " failed"); \
        ::test::fail(__FILE__, __LINE__, _buf); \
    } \
} while(0)

#define ASSERT_STREQ(a, b) do { \
    ::test::g_assertions++; \
    std::string _sa(a), _sb(b); \
    if (_sa != _sb) ::test::fail(__FILE__, __LINE__, (#a " == " #b " failed")); \
} while(0)

inline int runAll() {
    printf("\n=== ModernUI Unit Tests ===\n\n");
    int passed = 0, failed = 0;
    for (auto& tc : registry()) {
        ::test::g_assertions = 0;
        ::test::g_failures = 0;
        ::test::g_current_test = tc.name;
        try {
            tc.fn();
            if (::test::g_failures == 0) {
                printf("  PASS: %s (%d assertions)\n", tc.name, ::test::g_assertions);
                passed++;
            } else {
                failed++;
            }
        } catch (const std::exception& e) {
            printf("  FAIL: %s — %s\n", tc.name, e.what());
            failed++;
        } catch (...) {
            printf("  FAIL: %s — unknown exception\n", tc.name);
            failed++;
        }
    }
    printf("\n=== Results: %d passed, %d failed, %d total ===\n", passed, failed, passed + failed);
    return failed;
}

} // namespace test
