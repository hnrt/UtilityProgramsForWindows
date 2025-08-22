#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


int String::LastIndexOf(int c, SSIZE_T fromIndex) const
{
    SIZE_T length = Len;
    if (fromIndex < 0)
    {
        fromIndex += Len;
        if (fromIndex < 0)
        {
            return -1;
        }
    }
    if (length > fromIndex + 1)
    {
        length = fromIndex + 1;
    }
    if (m_psz && length)
    {
        return hnrt::LastIndexOf(m_psz, c, length);
    }
    return -1;
}


int String::LastIndexOf(const String& s, SSIZE_T fromIndex) const
{
    SIZE_T length = Len;
    if (fromIndex < 0)
    {
        fromIndex += Len;
        if (fromIndex < 0)
        {
            return -1;
        }
    }
    if (length > fromIndex + 1)
    {
        length = fromIndex + 1;
    }
    if (m_psz && length)
    {
        return hnrt::LastIndexOf(m_psz, s, length);
    }
    return -1;
}
