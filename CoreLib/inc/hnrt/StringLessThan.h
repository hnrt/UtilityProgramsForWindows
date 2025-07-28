#pragma once

#include "hnrt/String.h"

namespace hnrt
{
    class StringLessThan
    {
    public:

        bool operator ()(PCWSTR psz1, PCWSTR psz2) const;
    };

    inline bool StringLessThan::operator ()(PCWSTR psz1, PCWSTR psz2) const
    {
        return String::Compare(psz1, psz2) < 0;
    }
}
