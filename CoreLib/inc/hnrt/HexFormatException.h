#pragma once

#include <Windows.h>
#include "hnrt/FormatException.h"

namespace hnrt
{
    class HexFormatException
        : public FormatException
    {
    public:

        HexFormatException(SIZE_T uOffset, PCWSTR pszFormat, ...);
        HexFormatException(const HexFormatException& src);
        virtual ~HexFormatException() = default;
        HexFormatException& operator =(const HexFormatException& src);
    };
}
