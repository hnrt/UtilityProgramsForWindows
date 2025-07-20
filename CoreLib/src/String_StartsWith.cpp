#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


bool String::StartsWith(PCWSTR psz, SSIZE_T cch) const
{
    SIZE_T cchActual = StrLen(psz, cch);
    if (cchActual == 0)
    {
        return true;
    }
    else if (cchActual <= Len)
    {
        return Compare(m_psz, cchActual, psz, cchActual) == 0;
    }
    else
    {
        return false;
    }
}
