#pragma once


#include "hnrt/StringBuffer.h"


namespace hnrt
{
    struct StringStore
    {
        static PCWSTR Get(PCWSTR psz);
        static PCSTR Get(PCSTR psz);
        static PCWSTR Set(PWSTR psz);
        static PCSTR Set(PSTR psz);
        static PCWSTR Set(StringBuffer&);
    };
}
