#include "pch.h"
#include <cstdarg>
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefStr.h"


using namespace hnrt;


String::String(PCWSTR pszFormat, va_list argList)
    : m_psz(RefStr::Create(VaStrFmtLen(pszFormat, argList)))
{
    VaStrFmt(m_psz, Len + 1, pszFormat, argList);
}
