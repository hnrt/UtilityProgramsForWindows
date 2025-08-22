#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


long long String::ToLongLong(long long defaultValue, BOOL* pbSuccessful, int nRadix) const
{
    if (Len)
    {
        PWCHAR pStop = nullptr;
        long long value = StrToLongLong(&m_psz[0], &pStop, nRadix);
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
