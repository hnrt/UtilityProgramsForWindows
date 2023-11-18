#include "pch.h"
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/RefStr.h"
#include "hnrt/Interlocked.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G - C A S E - I N S E N S I T V E  C L A S S
//
//////////////////////////////////////////////////////////////////////


StringCaseInsensitive::StringCaseInsensitive()
    : m_ptr(nullptr)
{
}


StringCaseInsensitive::StringCaseInsensitive(PCWSTR psz)
    : m_ptr(psz ? new RefStr(Clone(psz)) : nullptr)
{
}


StringCaseInsensitive::StringCaseInsensitive(PCWSTR psz, size_t cb)
    : m_ptr(psz ? new RefStr(Clone(psz, cb)) : nullptr)
{
}


StringCaseInsensitive::StringCaseInsensitive(StringBuffer& buf)
    : m_ptr(new RefStr(buf))
{
}


StringCaseInsensitive::StringCaseInsensitive(RefStr* ptr)
    : m_ptr(ptr)
{
}


StringCaseInsensitive::StringCaseInsensitive(const String& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


StringCaseInsensitive::StringCaseInsensitive(const StringCaseInsensitive& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


StringCaseInsensitive::~StringCaseInsensitive()
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


StringCaseInsensitive& StringCaseInsensitive::operator =(const String& other)
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, other.m_ptr);
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


StringCaseInsensitive& StringCaseInsensitive::operator =(const StringCaseInsensitive& other)
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, other.m_ptr);
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
    if (ptr)
    {
        ptr->Release();
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
    return StringCaseInsensitive(new RefStr(Ptr, other.Ptr));
}


StringCaseInsensitive& StringCaseInsensitive::operator +=(const StringCaseInsensitive& other)
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(Ptr, other.Ptr));
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


PCWSTR StringCaseInsensitive::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : L"";
}


size_t StringCaseInsensitive::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
}


int StringCaseInsensitive::Compare(PCWSTR psz1, LONG_PTR cch1, PCWSTR psz2, LONG_PTR cch2)
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
        throw Exception(L"Compare failed.");
    }
}
