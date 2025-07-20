#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::VaFormat(PCWSTR pszFormat, va_list argList)
{
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(pszFormat, argList)));
    return *this;
}
