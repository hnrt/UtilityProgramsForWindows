#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


int String::IndexOf(int c, SIZE_T fromIndex) const
{
    SIZE_T length = Len;
    if (m_psz && fromIndex < length)
    {
        int index = hnrt::IndexOf(m_psz + fromIndex, c, length - fromIndex);
        return index >= 0 ? (static_cast<int>(fromIndex) + index) : -1;
    }
    return -1;
}


int String::IndexOf(const String& s, SIZE_T fromIndex) const
{
    SIZE_T length = Len;
    if (m_psz && fromIndex < length)
    {
        int index = hnrt::IndexOf(m_psz + fromIndex, s, length - fromIndex);
        return index >= 0 ? (static_cast<int>(fromIndex) + index) : -1;
    }
    return -1;
}
