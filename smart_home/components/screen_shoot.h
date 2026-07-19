#ifndef LV_SCREENSHOT_H
#define LV_SCREENSHOT_H

#include <string>

class ScreenShot
{
public:
    ScreenShot() = default;
    ~ScreenShot() = default;

    static void take(const std::string &path);
};

#endif// LV_SCREENSHOT_H