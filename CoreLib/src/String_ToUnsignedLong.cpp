#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


unsigned long String::ToUnsignedLong(unsigned long defaultValue, BOOL* pbSuccessful, int nRadix) const
{
    if (Len)
    {
        PWCHAR pStop = nullptr;
        unsigned long value = StrToUnsignedLong(&m_psz[0], &pStop, nRadix);
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
