#pragma once

#include "hnrt/StringCaseInsensitive.h"

namespace hnrt
{
    class StringCaseLessThan
    {
    public:

        bool operator ()(PCWSTR psz1, PCWSTR psz2) const;
    };

    inline bool StringCaseLessThan::operator ()(PCWSTR psz1, PCWSTR psz2) const
    {
        return StringCaseInsensitive::Compare(psz1, psz2) < 0;
    }
}
