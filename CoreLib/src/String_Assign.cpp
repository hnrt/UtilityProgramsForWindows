#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Interlocked.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"


using namespace hnrt;


String& String::Assign(const String& src)
{
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, StringAddRef(src.m_psz)));
    return *this;
}


String& String::Assign(PCWSTR psz, SSIZE_T cch)
{
    cch = StrLen(psz, cch);
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, RefStr::Create(cch)));
    m_psz[MemCpy(m_psz, psz, cch)] = L'\0';
    return *this;
}
