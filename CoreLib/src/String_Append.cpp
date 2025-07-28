#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::Append(const String& src)
{
    if (src.Len)
    {
        SIZE_T cch1 = Len;
        SIZE_T cch2 = src.Len;
        PWSTR psz = RefStr::Create(cch1 + cch2);
        MemCpy(psz, m_psz, cch1);
        MemCpy(psz + cch1, src.m_psz, cch2 + 1);
        StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, psz));
    }
    return *this;
}


String& String::Append(PCWSTR psz, SSIZE_T cch)
{
    if (psz && cch)
    {
        SIZE_T cch1 = Len;
        SIZE_T cch2 = StrLen(psz, cch);
        PWSTR psz3 = RefStr::Create(cch1 + cch2);
        MemCpy(psz3, m_psz, cch1);
        MemCpy(psz3 + cch1, psz, cch2);
        psz3[cch1 + cch2] = L'\0';
        StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, psz3));
    }
    return *this;
}
