#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::Format(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    SIZE_T cch = VaStrFmtLen(pszFormat, argList);
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(cch)));
    VaStrFmt(m_psz, cch + 1, pszFormat, argList);
    va_end(argList);
    return *this;
}
