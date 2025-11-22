#include "pch.h"
#include <Windows.h>
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"
#include "hnrt/String.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


StringCaseInsensitive& StringCaseInsensitive::operator =(const StringCaseInsensitive& src)
{
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, StringAddRef(src.m_psz)));
    return *this;
}


StringCaseInsensitive& StringCaseInsensitive::operator =(const String& src)
{
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, StringAddRef(src.Buf)));
    return *this;
}


StringCaseInsensitive& StringCaseInsensitive::operator +=(const StringCaseInsensitive& src)
{
    if (src.Len)
    {
        PWSTR psz = RefStr::Create(Len + src.Len);
        MemCpy(psz, m_psz, Len);
        MemCpy(psz + Len, src.Ptr, src.Len + 1);
        StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, psz));
    }
    return *this;
}


StringCaseInsensitive& StringCaseInsensitive::operator +=(const String& src)
{
    if (src.Len)
    {
        PWSTR psz = RefStr::Create(Len + src.Len);
        MemCpy(psz, m_psz, Len);
        MemCpy(psz + Len, src.Ptr, src.Len + 1);
        StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, psz));
    }
    return *this;
}


bool StringCaseInsensitive::operator ==(const StringCaseInsensitive& src) const
{
    return StrCaseCmp(Ptr, -1, src.Ptr, -1) == 0;
}


bool StringCaseInsensitive::operator !=(const StringCaseInsensitive& src) const
{
    return StrCaseCmp(Ptr, -1, src.Ptr, -1) != 0;
}


bool StringCaseInsensitive::operator <(const StringCaseInsensitive& src) const
{
    return StrCaseCmp(Ptr, -1, src.Ptr, -1) < 0;
}


bool StringCaseInsensitive::operator <=(const StringCaseInsensitive& src) const
{
    return StrCaseCmp(Ptr, -1, src.Ptr, -1) <= 0;
}


bool StringCaseInsensitive::operator >(const StringCaseInsensitive& src) const
{
    return StrCaseCmp(Ptr, -1, src.Ptr, -1) > 0;
}


bool StringCaseInsensitive::operator >=(const StringCaseInsensitive& src) const
{
    return StrCaseCmp(Ptr, -1, src.Ptr, -1) >= 0;
}


StringCaseInsensitive StringCaseInsensitive::operator +(const StringCaseInsensitive& src) const
{
    return StringCaseInsensitive(String(Ptr, src.Ptr));
}


StringCaseInsensitive StringCaseInsensitive::operator +(const String& src) const
{
    return StringCaseInsensitive(String(Ptr, src.Ptr));
}
