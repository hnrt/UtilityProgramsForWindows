#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


double String::ToDouble(double defaultValue, BOOL* pbSuccessful) const
{
    if (Len)
    {
        PWCHAR pStop = nullptr;
        double value = StrToDouble(&m_psz[0], &pStop);
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
