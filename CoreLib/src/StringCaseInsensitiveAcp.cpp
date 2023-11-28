#include "pch.h"
#include "hnrt/StringCaseInsensitiveAcp.h"
#include "hnrt/RefMbs.h"
#include "hnrt/StringAcp.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G - C A S E - I N S E N S I T V E - A C P  C L A S S
//
//////////////////////////////////////////////////////////////////////


inline PCSTR AddRef(PCSTR psz)
{
    if (psz)
    {
        RefMbs::Get(psz).AddRef();
    }
    return psz;
}


inline void Release(PCSTR psz)
{
    if (psz)
    {
        RefMbs::Get(psz).Release();
    }
}


StringCaseInsensitiveAcp::StringCaseInsensitiveAcp()
    : m_psz(nullptr)
{
}


StringCaseInsensitiveAcp::StringCaseInsensitiveAcp(const StringCaseInsensitiveAcp& other)
    : m_psz(AddRef(other.m_psz))
{
}


StringCaseInsensitiveAcp::StringCaseInsensitiveAcp(PCSTR psz, INT_PTR cb)
    : m_psz(RefMbs::Create(psz, cb))
{
}


StringCaseInsensitiveAcp::StringCaseInsensitiveAcp(const StringAcp& other)
    : m_psz(AddRef(other.m_psz))
{
}


StringCaseInsensitiveAcp::~StringCaseInsensitiveAcp()
{
    Release(InterlockedExchangePCSTR(&m_psz, nullptr));
}


StringCaseInsensitiveAcp& StringCaseInsensitiveAcp::operator =(const StringCaseInsensitiveAcp& other)
{
    Release(InterlockedExchangePCSTR(&m_psz, AddRef(other.m_psz)));
    return *this;
}


StringCaseInsensitiveAcp& StringCaseInsensitiveAcp::operator =(const StringAcp& other)
{
    Release(InterlockedExchangePCSTR(&m_psz, AddRef(other.m_psz)));
    return *this;
}


StringCaseInsensitiveAcp& StringCaseInsensitiveAcp::operator +=(const StringCaseInsensitiveAcp& other)
{
    if (other.Len)
    {
        Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(Ptr, other.Ptr)));
    }
    return *this;
}


StringCaseInsensitiveAcp& StringCaseInsensitiveAcp::operator +=(const StringAcp& other)
{
    if (other.Len)
    {
        Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(Ptr, other.Ptr)));
    }
    return *this;
}


bool StringCaseInsensitiveAcp::operator ==(const StringCaseInsensitiveAcp& other) const
{
    return Compare(Ptr, other.Ptr) == 0;
}


bool StringCaseInsensitiveAcp::operator !=(const StringCaseInsensitiveAcp& other) const
{
    return Compare(Ptr, other.Ptr) != 0;
}


bool StringCaseInsensitiveAcp::operator <(const StringCaseInsensitiveAcp& other) const
{
    return Compare(Ptr, other.Ptr) < 0;
}


bool StringCaseInsensitiveAcp::operator <=(const StringCaseInsensitiveAcp& other) const
{
    return Compare(Ptr, other.Ptr) <= 0;
}


bool StringCaseInsensitiveAcp::operator >(const StringCaseInsensitiveAcp& other) const
{
    return Compare(Ptr, other.Ptr) > 0;
}


bool StringCaseInsensitiveAcp::operator >=(const StringCaseInsensitiveAcp& other) const
{
    return Compare(Ptr, other.Ptr) >= 0;
}


StringCaseInsensitiveAcp StringCaseInsensitiveAcp::operator +(const StringCaseInsensitiveAcp& other) const
{
    return StringCaseInsensitiveAcp(StringAcp(Ptr, other.Ptr));
}


StringCaseInsensitiveAcp StringCaseInsensitiveAcp::operator +(const StringAcp& other) const
{
    return StringCaseInsensitiveAcp(StringAcp(Ptr, other.Ptr));
}


PCSTR StringCaseInsensitiveAcp::get_ptr() const
{
    return m_psz ? m_psz : "";
}


size_t StringCaseInsensitiveAcp::get_len() const
{
    return m_psz ? RefMbs::Get(m_psz).Len : 0;
}


int StringCaseInsensitiveAcp::Compare(PCSTR psz1, INT_PTR cch1, PCSTR psz2, INT_PTR cch2)
{
    switch (CompareStringA(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
    {
    case CSTR_EQUAL:
        return 0;
    case CSTR_LESS_THAN:
        return -1;
    case CSTR_GREATER_THAN:
        return 1;
    default:
        throw Exception(L"StringCaseInsensitiveAcp::Compare failed.");
    }
}
