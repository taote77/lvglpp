

#ifndef LVGLPP_WAYLANDAPPLICATION_H
#define LVGLPP_WAYLANDAPPLICATION_H
#ifdef USED_WAYLAND

#include "Application.h"
namespace heygears
{
    namespace sys
    {
        class WaylandApplication:public Application
        {
        public:
            WaylandApplication(int argc, char *argv[]);

            virtual bool initApp() override;

            virtual void exit(int c) override;
        };

    } // heygears
} // sys
#endif
#endif //LVGLPP_WAYLANDAPPLICATION_H
