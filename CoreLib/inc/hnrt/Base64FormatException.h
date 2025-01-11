#pragma once

#include <Windows.h>
#include "hnrt/FormatException.h"

namespace hnrt
{
    class Base64FormatException
        : public FormatException
    {
    public:

        Base64FormatException(SIZE_T uOffset, PCWSTR pszFormat, ...);
        Base64FormatException(const Base64FormatException& src);
        virtual ~Base64FormatException() = default;
        Base64FormatException& operator =(const Base64FormatException& src);
    };
}
