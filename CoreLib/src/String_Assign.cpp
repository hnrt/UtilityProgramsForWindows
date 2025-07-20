#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::Assign(const String& src)
{
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, AddRef(src.m_psz)));
    return *this;
}


String& String::Assign(PCWSTR psz, SSIZE_T cch)
{
    Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(psz, cch)));
    return *this;
}
