#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::Append(const String& src)
{
    if (src.Len)
    {
        Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, src.Ptr)));
    }
    return *this;
}


String& String::Append(PCWSTR psz, SSIZE_T cch)
{
    if (psz && cch)
    {
        Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(Ptr, psz, cch)));
    }
    return *this;
}
