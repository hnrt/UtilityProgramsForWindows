#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


bool String::EndsWith(PCWSTR psz, SSIZE_T cch) const
{
    SIZE_T cchActual = StrLen(psz, cch);
    if (cchActual == 0)
    {
        return true;
    }
    else if (cchActual <= Len)
    {
        return Compare(m_psz + Len - cchActual, cchActual, psz, cchActual) == 0;
    }
    else
    {
        return false;
    }
}
