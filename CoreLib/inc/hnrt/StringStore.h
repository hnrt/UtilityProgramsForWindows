#pragma once


#include "hnrt/StringBuffer.h"


namespace hnrt
{
    struct StringStore
    {
        static PCWSTR Get(PCWSTR psz);
        static PCSTR Get(PCSTR psz);
    };
}
