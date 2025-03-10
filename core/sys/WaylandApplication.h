#ifndef ULTRACORE_FOR_MAKERS_WAYLANDAPPLICATION_H
#define ULTRACORE_FOR_MAKERS_WAYLANDAPPLICATION_H
#ifdef USED_WAYLAND
#  include "Application.h"
namespace heygears {
namespace sys {
class WaylandApplication : public Application
{
public:
    WaylandApplication(int argc, char *argv[]);

private:
protected:
    bool initApp() override;

    void exit(int c) override;
};

} // namespace sys
} // namespace heygears
#endif
#endif // ULTRACORE_FOR_MAKERS_WAYLANDAPPLICATION_H
