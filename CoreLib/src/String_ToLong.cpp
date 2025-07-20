#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


long String::ToLong(long defaultValue, BOOL* pbSuccessful, int nRadix)
{
    if (Len)
    {
        PWCHAR pStop = nullptr;
        long value = StrToLong(&m_psz[0], &pStop, nRadix);
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
