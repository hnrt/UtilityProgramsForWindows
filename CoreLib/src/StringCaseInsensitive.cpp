#include "pch.h"
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/RefStr.h"
#include "hnrt/String.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G - C A S E - I N S E N S I T V E  C L A S S
//
//////////////////////////////////////////////////////////////////////


StringCaseInsensitive::StringCaseInsensitive()
    : m_psz(nullptr)
{
}


StringCaseInsensitive::StringCaseInsensitive(const StringCaseInsensitive& other)
    : m_psz(other.m_psz)
{
    if (m_psz)
    {
        RefStr::Get(m_psz).AddRef();
    }
}


StringCaseInsensitive::StringCaseInsensitive(PCWSTR psz, INT_PTR cch)
    : m_psz(psz&& cch ? RefStr::Create(psz, cch) : nullptr)
{
}


StringCaseInsensitive::StringCaseInsensitive(const String& other)
    : m_psz(other.m_psz)
{
    if (m_psz)
    {
        RefStr::Get(m_psz).AddRef();
    }
}


StringCaseInsensitive::~StringCaseInsensitive()
{
    PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, nullptr);
    if (psz)
    {
        RefStr::Get(psz).Release();
    }
}


StringCaseInsensitive& StringCaseInsensitive::operator =(const StringCaseInsensitive& other)
{
    PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, other.m_psz);
    if (m_psz)
    {
        RefStr::Get(m_psz).AddRef();
    }
    if (psz)
    {
        RefStr::Get(psz).Release();
    }
    return *this;
}


StringCaseInsensitive& StringCaseInsensitive::operator =(const String& other)
{
    PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, other.m_psz);
    if (m_psz)
    {
        RefStr::Get(m_psz).AddRef();
    }
    if (psz)
    {
        RefStr::Get(psz).Release();
    }
    return *this;
}


StringCaseInsensitive& StringCaseInsensitive::operator +=(const StringCaseInsensitive& other)
{
    if (other.Len)
    {
        PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, RefStr::Create(Ptr, other.Ptr));
        if (psz)
        {
            RefStr::Get(psz).Release();
        }
    }
    return *this;
}


StringCaseInsensitive& StringCaseInsensitive::operator +=(const String& other)
{
    if (other.Len)
    {
        PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, RefStr::Create(Ptr, other.Ptr));
        if (psz)
        {
            RefStr::Get(psz).Release();
        }
    }
    return *this;
}


bool StringCaseInsensitive::operator ==(const StringCaseInsensitive& other) const
{
    return Compare(*this, other) == 0;
}


bool StringCaseInsensitive::operator !=(const StringCaseInsensitive& other) const
{
    return Compare(*this, other) != 0;
}


bool StringCaseInsensitive::operator <(const StringCaseInsensitive& other) const
{
    return Compare(*this, other) < 0;
}


bool StringCaseInsensitive::operator <=(const StringCaseInsensitive& other) const
{
    return Compare(*this, other) <= 0;
}


bool StringCaseInsensitive::operator >(const StringCaseInsensitive& other) const
{
    return Compare(*this, other) > 0;
}


bool StringCaseInsensitive::operator >=(const StringCaseInsensitive& other) const
{
    return Compare(*this, other) >= 0;
}


StringCaseInsensitive StringCaseInsensitive::operator +(const StringCaseInsensitive& other) const
{
    return StringCaseInsensitive(String(Ptr, other.Ptr));
}


StringCaseInsensitive StringCaseInsensitive::operator +(const String& other) const
{
    return StringCaseInsensitive(String(Ptr, other.Ptr));
}


PCWSTR StringCaseInsensitive::get_ptr() const
{
    return m_psz ? m_psz : L"";
}


size_t StringCaseInsensitive::get_len() const
{
    return m_psz ? RefStr::Get(m_psz).Len : 0;
}


int StringCaseInsensitive::Compare(PCWSTR psz1, INT_PTR cch1, PCWSTR psz2, INT_PTR cch2)
{
    switch (CompareStringW(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
    {
    case CSTR_EQUAL:
        return 0;
    case CSTR_LESS_THAN:
        return -1;
    case CSTR_GREATER_THAN:
        return 1;
    default:
        throw Exception(L"StringCaseInsensitive::Compare failed.");
    }
}
