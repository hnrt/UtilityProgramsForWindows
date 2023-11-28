#include "pch.h"
#include "hnrt/StringUTF8.h"
#include "hnrt/RefMbs.h"
#include "hnrt/StringAcp.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G - U T F 8   C L A S S
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


StringUTF8::StringUTF8()
    : m_psz(nullptr)
{
}


StringUTF8::StringUTF8(const StringUTF8& other)
    : m_psz(AddRef(other.m_psz))
{
}


StringUTF8::StringUTF8(PCSTR psz, INT_PTR cb)
    : m_psz(RefMbs::Create(psz, cb))
{
}


StringUTF8::StringUTF8(PCWSTR psz, INT_PTR cch)
    : m_psz(RefMbs::Create(CP_UTF8, psz, cch))
{
}


StringUTF8::StringUTF8(RefMbs* ptr)
    : m_psz(RefMbs::Get(ptr))
{
}


StringUTF8::~StringUTF8()
{
    Release(InterlockedExchangePCSTR(&m_psz, nullptr));
}


StringUTF8& StringUTF8::operator =(const StringUTF8& other)
{
    Release(InterlockedExchangePCSTR(&m_psz, AddRef(other.m_psz)));
    return *this;
}


StringUTF8& StringUTF8::operator +=(const StringUTF8& other)
{
    if (other.Len)
    {
        Release(InterlockedExchangePCSTR(&m_psz, RefMbs::Create(Ptr, other.Ptr)));
    }
    return *this;
}


bool StringUTF8::operator ==(const StringUTF8& other) const
{
    return strcmp(Ptr, other.Ptr) == 0;
}


bool StringUTF8::operator !=(const StringUTF8& other) const
{
    return strcmp(Ptr, other.Ptr) != 0;
}


bool StringUTF8::operator <(const StringUTF8& other) const
{
    return strcmp(Ptr, other.Ptr) < 0;
}


bool StringUTF8::operator <=(const StringUTF8& other) const
{
    return strcmp(Ptr, other.Ptr) <= 0;
}


bool StringUTF8::operator >(const StringUTF8& other) const
{
    return strcmp(Ptr, other.Ptr) > 0;
}


bool StringUTF8::operator >=(const StringUTF8& other) const
{
    return strcmp(Ptr, other.Ptr) >= 0;
}


StringUTF8 StringUTF8::operator +(const StringUTF8& other) const
{
    return StringUTF8(RefMbs::Create(Ptr, other.Ptr));
}


PCSTR StringUTF8::get_ptr() const
{
    return m_psz ? m_psz : "";
}


size_t StringUTF8::get_len() const
{
    return m_psz ? RefMbs::Get(m_psz).Len : 0;
}
