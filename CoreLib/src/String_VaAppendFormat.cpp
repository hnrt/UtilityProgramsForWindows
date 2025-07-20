#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::VaAppendFormat(PCWSTR pszFormat, va_list argList)
{
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, pszFormat, argList)));
    return *this;
}
