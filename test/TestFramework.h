#ifndef LVGLPP_TEST_FRAMEWORK_H
#define LVGLPP_TEST_FRAMEWORK_H

#include <cassert>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace lvglpp::test {

struct TestResult {
    std::string name;
    bool passed;
    std::string message;
};

class TestRunner
{
public:
    static TestRunner &instance()
    {
        static TestRunner tr;
        return tr;
    }

    void addTest(const std::string &name, std::function<bool()> fn)
    {
        _tests.push_back({name, fn});
    }

    int runAll()
    {
        int passed = 0, failed = 0;
        std::cout << "\n=== LVGL++ Unit Tests ===\n\n";
        for (auto &t : _tests) {
            std::cout << "  " << t.first << " ... ";
            try {
                if (t.second()) {
                    std::cout << "PASSED\n";
                    passed++;
                } else {
                    std::cout << "FAILED\n";
                    failed++;
                }
            } catch (const std::exception &e) {
                std::cout << "ERROR: " << e.what() << "\n";
                failed++;
            }
        }
        std::cout << "\n--- Results: " << passed << " passed, " << failed << " failed ---\n";
        return failed;
    }

private:
    std::vector<std::pair<std::string, std::function<bool()>>> _tests;
};

#define TEST(name) \
    static bool test_##name(); \
    namespace { \
        struct _Register_##name { \
            _Register_##name() { \
                TestRunner::instance().addTest(#name, test_##name); \
            } \
        } _reg_##name; \
    } \
    static bool test_##name()

#define CHECK(expr) do { if (!(expr)) { std::cerr << "  FAIL at " << __FILE__ << ":" << __LINE__ << " — " #expr << std::endl; return false; } } while(0)
#define CHECK_EQ(a, b) do { if ((a) != (b)) { std::cerr << "  FAIL: " << (a) << " != " << (b) << " at " << __FILE__ << ":" << __LINE__ << std::endl; return false; } } while(0)
#define CHECK_TRUE(expr) CHECK(expr)
#define CHECK_FALSE(expr) CHECK(!(expr))

} // namespace lvglpp::test

#endif // LVGLPP_TEST_FRAMEWORK_H
