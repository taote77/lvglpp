#ifndef LVGLPP_WAYLANDAPPLICATION_H
#define LVGLPP_WAYLANDAPPLICATION_H

#include "../Application.h"

namespace lvglpp::sys {
class WaylandApplication : public Application
{
public:
    WaylandApplication(int argc, char *argv[]);

    virtual bool initApp() override;

    virtual void exit(int c) override;
};

} // namespace lvglpp::sys

#endif // LVGLPP_WAYLANDAPPLICATION_H
