#pragma once


#include "hnrt/ICabinet.h"


namespace hnrt
{
    struct CabinetFactory
    {
        static ICabinet* Create();
    };
}
