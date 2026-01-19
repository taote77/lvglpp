#ifndef LVGLPP_PYTHON_UTILS_H
#define LVGLPP_PYTHON_UTILS_H

#include <string>

namespace lvglpp::python {

class PythonUtils {
public:
    static void printInfo(const std::string& message);
    static void printError(const std::string& message);
    static void printDebug(const std::string& message);
    
    // Format a string with arguments (similar to printf)
    template<typename... Args>
    static std::string format(const char* format, Args... args);
};

// Template implementation
template<typename... Args>
std::string PythonUtils::format(const char* format, Args... args) {
    int size = snprintf(nullptr, 0, format, args...) + 1; // +1 for null terminator
    char* buf = new char[size];
    snprintf(buf, size, format, args...);
    std::string result(buf);
    delete[] buf;
    return result;
}

} // namespace lvglpp::python

#endif // LVGLPP_PYTHON_UTILS_H
