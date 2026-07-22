#ifndef SMART_HOME_DEVICE_CONTROL_PAGE_H
#define SMART_HOME_DEVICE_CONTROL_PAGE_H

#include "core/sys/BaseActivity.h"
#include "core/widgets/LvText.h"
#include <memory>
#include <vector>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::page {

/// Device control page showing all smart devices grouped by type
class DeviceControlPage : public sys::BaseActivity
{
public:
    DeviceControlPage();
    ~DeviceControlPage() override;

protected:
    void onCreate(std::any arg) override;
    void onNotifyUI(const sys::Event &evt) override;
    void onDestroy() override;

private:
    void buildLightsSection(int &y);
    void buildThermostatsSection(int &y);
    void buildLocksSection(int &y);

    std::unique_ptr<w::LvText> _title;
};

} // namespace lvglpp::ui::page

#endif
