#pragma once


#include "hnrt/StringBuffer.h"


namespace hnrt
{
    class String;
    class MultibyteString;

    struct StringStore
    {
        static String Get(PCWSTR psz);
        static MultibyteString Get(PCSTR psz);
    };
}
