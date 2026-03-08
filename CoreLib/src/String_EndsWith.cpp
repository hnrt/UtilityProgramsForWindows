#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


bool String::EndsWith(PCWSTR psz, SSIZE_T cch) const
{
    SIZE_T cchActual = StrLen(psz, cch);
    if (cchActual == 0)
    {
        return true;
    }
    else if (cchActual <= Length)
    {
        return Compare(m_psz + Length - cchActual, cchActual, psz, cchActual) == 0;
    }
    else
    {
        return false;
    }
}
