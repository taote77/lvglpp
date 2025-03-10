/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-06-23

Class:${CLASS}

Description:

**************************************************************************/

#include "Event.h"

namespace heygears
{
    namespace sys
    {
        Event::Event(int type, int error_code,boost::any data) : type_(type), data_(std::move(data)),error_code_(error_code)
        {
        }

        Event::Event(int type, int error_code): type_(type),error_code_(error_code)
        {

        }
    }
} // sys
