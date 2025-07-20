#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::Format(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(pszFormat, argList)));
    va_end(argList);
    return *this;
}
