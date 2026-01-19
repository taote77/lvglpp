#include "PythonUtils.h"
#include <iostream>
#include <iomanip>
#include <chrono>

namespace lvglpp::python {

void PythonUtils::printInfo(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto local_time = std::localtime(&now_time);
    
    std::cout << "[INFO] "
              << std::put_time(local_time, "%Y-%m-%d %H:%M:%S")
              << " - " << message << std::endl;
}

void PythonUtils::printError(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto local_time = std::localtime(&now_time);
    
    std::cerr << "[ERROR] "
              << std::put_time(local_time, "%Y-%m-%d %H:%M:%S")
              << " - " << message << std::endl;
}

void PythonUtils::printDebug(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto local_time = std::localtime(&now_time);
    
    std::cout << "[DEBUG] "
              << std::put_time(local_time, "%Y-%m-%d %H:%M:%S")
              << " - " << message << std::endl;
}

} // namespace lvglpp::python
