#ifndef QGL_TEST_FRAMEWORK_H
#define QGL_TEST_FRAMEWORK_H

#include "../QGL.h"
#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <chrono>

namespace QGL {
namespace Test {

// 测试结果
struct TestResult {
    std::string testName;
    bool passed;
    std::string errorMessage;
    double executionTime; // 毫秒
};

// 测试套件
class TestSuite {
public:
    TestSuite(const std::string& name) : m_name(name) {}
    
    void addTest(const std::string& testName, std::function<void()> testFunc) {
        m_tests.emplace_back(testName, testFunc);
    }
    
    std::vector<TestResult> run() {
        std::vector<TestResult> results;
        
        std::cout << "运行测试套件: " << m_name << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        for (const auto& test : m_tests) {
            TestResult result;
            result.testName = test.first;
            
            auto start = std::chrono::high_resolution_clock::now();
            
            try {
                test.second(); // 执行测试
                result.passed = true;
                std::cout << "[PASS] " << test.first << std::endl;
            } catch (const std::exception& e) {
                result.passed = false;
                result.errorMessage = e.what();
                std::cout << "[FAIL] " << test.first << " - " << e.what() << std::endl;
            } catch (...) {
                result.passed = false;
                result.errorMessage = "Unknown exception";
                std::cout << "[FAIL] " << test.first << " - Unknown exception" << std::endl;
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            result.executionTime = duration.count() / 1000.0;
            
            results.push_back(result);
        }
        
        return results;
    }
    
    std::string getName() const { return m_name; }
    
private:
    std::string m_name;
    std::vector<std::pair<std::string, std::function<void()>>> m_tests;
};

// 测试断言
class Assert {
public:
    static void isTrue(bool condition, const std::string& message = "Assertion failed") {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }
    
    static void isFalse(bool condition, const std::string& message = "Assertion failed") {
        if (condition) {
            throw std::runtime_error(message);
        }
    }
    
    template<typename T>
    static void areEqual(const T& expected, const T& actual, const std::string& message = "Values are not equal") {
        if (!(expected == actual)) {
            throw std::runtime_error(message);
        }
    }
    
    template<typename T>
    static void areNotEqual(const T& expected, const T& actual, const std::string& message = "Values are equal") {
        if (expected == actual) {
            throw std::runtime_error(message);
        }
    }
    
    static void isNull(void* ptr, const std::string& message = "Pointer is not null") {
        if (ptr != nullptr) {
            throw std::runtime_error(message);
        }
    }
    
    static void isNotNull(void* ptr, const std::string& message = "Pointer is null") {
        if (ptr == nullptr) {
            throw std::runtime_error(message);
        }
    }
    
    static void throws(std::function<void()> func, const std::string& message = "Exception was not thrown") {
        bool exceptionThrown = false;
        try {
            func();
        } catch (...) {
            exceptionThrown = true;
        }
        
        if (!exceptionThrown) {
            throw std::runtime_error(message);
        }
    }
};

// 测试运行器
class TestRunner {
public:
    static TestRunner& instance() {
        static TestRunner runner;
        return runner;
    }
    
    void addSuite(TestSuite* suite) {
        m_suites.push_back(suite);
    }
    
    void runAll() {
        int totalTests = 0;
        int passedTests = 0;
        int failedTests = 0;
        double totalTime = 0.0;
        
        std::cout << "========================================" << std::endl;
        std::cout << "开始运行 QGL 框架测试" << std::endl;
        std::cout << "========================================" << std::endl;
        
        for (TestSuite* suite : m_suites) {
            std::vector<TestResult> results = suite->run();
            
            for (const TestResult& result : results) {
                totalTests++;
                totalTime += result.executionTime;
                
                if (result.passed) {
                    passedTests++;
                } else {
                    failedTests++;
                }
            }
            
            std::cout << std::endl;
        }
        
        std::cout << "========================================" << std::endl;
        std::cout << "测试完成" << std::endl;
        std::cout << "总测试数: " << totalTests << std::endl;
        std::cout << "通过: " << passedTests << std::endl;
        std::cout << "失败: " << failedTests << std::endl;
        std::cout << "成功率: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0) << "%" << std::endl;
        std::cout << "总耗时: " << totalTime << " ms" << std::endl;
        std::cout << "========================================" << std::endl;
    }
    
private:
    std::vector<TestSuite*> m_suites;
};

// 便捷宏
#define ASSERT_TRUE(condition) Assert::isTrue(condition, #condition)
#define ASSERT_FALSE(condition) Assert::isFalse(condition, #condition)
#define ASSERT_EQUAL(expected, actual) Assert::areEqual(expected, actual, #expected " == " #actual)
#define ASSERT_NOT_EQUAL(expected, actual) Assert::areNotEqual(expected, actual, #expected " != " #actual)
#define ASSERT_NULL(ptr) Assert::isNull(ptr, #ptr " is null")
#define ASSERT_NOT_NULL(ptr) Assert::isNotNull(ptr, #ptr " is not null")
#define ASSERT_THROWS(func) Assert::throws(func, #func " throws exception")

} // namespace Test
} // namespace QGL

#endif // QGL_TEST_FRAMEWORK_H