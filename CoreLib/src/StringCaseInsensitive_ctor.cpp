#include "pch.h"
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/RefStr.h"
#include "hnrt/String.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


StringCaseInsensitive::StringCaseInsensitive()
    : m_psz(nullptr)
{
}


StringCaseInsensitive::StringCaseInsensitive(const StringCaseInsensitive& src)
    : m_psz(StringAddRef(src.m_psz))
{
}


StringCaseInsensitive::StringCaseInsensitive(PCWSTR psz, SSIZE_T cch)
    : m_psz(nullptr)
{
    cch = StrLen(psz, cch);
    m_psz = RefStr::Create(cch);
    MemCpy(m_psz, psz, cch);
    m_psz[cch] = L'\0';
}


StringCaseInsensitive::StringCaseInsensitive(const String& src)
    : m_psz(StringAddRef(src.Buf))
{
}


SIZE_T StringCaseInsensitive::get_Len() const
{
    return m_psz ? RefStr::Get(m_psz).Len : 0;
}
