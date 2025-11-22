#include "pch.h"
#include <cstdarg>
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Interlocked.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"


using namespace hnrt;


String String::Format(PCWSTR pszFormat, ...)
{
    String str;
    va_list argList;
    va_start(argList, pszFormat);
    SIZE_T cch = VaStrFmtLen(pszFormat, argList);
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&str.m_psz, RefStr::Create(cch)));
    VaStrFmt(str.m_psz, cch + 1, pszFormat, argList);
    va_end(argList);
    return str;
}
