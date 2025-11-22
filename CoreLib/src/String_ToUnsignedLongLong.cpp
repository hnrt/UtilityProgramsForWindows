#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


unsigned long long String::ToUnsignedLongLong(unsigned long long defaultValue, BOOL* pbSuccessful, int nRadix) const
{
    if (Len)
    {
        PWCHAR pStop = nullptr;
        unsigned long long value = StrToUnsignedLongLong(&m_psz[0], &pStop, nRadix);
        if (!*pStop)
        {
            if (pbSuccessful)
            {
                *pbSuccessful = TRUE;
            }
            return value;
        }
    }
    if (pbSuccessful)
    {
        *pbSuccessful = FALSE;
    }
    return defaultValue;
}
