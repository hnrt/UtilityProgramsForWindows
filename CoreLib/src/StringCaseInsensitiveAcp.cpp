#include "pch.h"
#include "hnrt/StringCaseInsensitiveAcp.h"
#include "hnrt/RefMbs.h"
#include "hnrt/StringAcp.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G - C A S E - I N S E N S I T V E - A C P  C L A S S
//
//////////////////////////////////////////////////////////////////////


StringCaseInsensitiveAcp::StringCaseInsensitiveAcp()
    : m_ptr(nullptr)
{
}


StringCaseInsensitiveAcp::StringCaseInsensitiveAcp(const StringCaseInsensitiveAcp& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


StringCaseInsensitiveAcp::StringCaseInsensitiveAcp(PCSTR psz, INT_PTR cch)
    : m_ptr(psz&& cch ? new RefMbs(psz, cch) : nullptr)
{
}


StringCaseInsensitiveAcp::StringCaseInsensitiveAcp(const StringAcp& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


StringCaseInsensitiveAcp::~StringCaseInsensitiveAcp()
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


StringCaseInsensitiveAcp& StringCaseInsensitiveAcp::operator =(const StringCaseInsensitiveAcp& other)
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, other.m_ptr);
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


StringCaseInsensitiveAcp& StringCaseInsensitiveAcp::operator =(const StringAcp& other)
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, other.m_ptr);
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


StringCaseInsensitiveAcp& StringCaseInsensitiveAcp::operator +=(const StringCaseInsensitiveAcp& other)
{
    if (other.Len)
    {
        if (m_ptr && m_ptr->RefCnt == 1)
        {
            m_ptr->Append(other.Ptr);
        }
        else
        {
            PSTR psz2 = Allocate<CHAR>(Len + other.Len + 1);
            memcpy_s(psz2, Len, Ptr, Len);
            memcpy_s(psz2 + Len, other.Len, other.Ptr, other.Len);
            psz2[Len + other.Len] = '\0';
            RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(IMMEDIATE_TEXT, psz2));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


StringCaseInsensitiveAcp& StringCaseInsensitiveAcp::operator +=(const StringAcp& other)
{
    if (other.Len)
    {
        if (m_ptr && m_ptr->RefCnt == 1)
        {
            m_ptr->Append(other.Ptr);
        }
        else
        {
            PSTR psz2 = Allocate<CHAR>(Len + other.Len + 1);
            memcpy_s(psz2, Len, Ptr, Len);
            memcpy_s(psz2 + Len, other.Len, other.Ptr, other.Len);
            psz2[Len + other.Len] = '\0';
            RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(IMMEDIATE_TEXT, psz2));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


bool StringCaseInsensitiveAcp::operator ==(const StringCaseInsensitiveAcp& other) const
{
    return Compare(*this, other) == 0;
}


bool StringCaseInsensitiveAcp::operator !=(const StringCaseInsensitiveAcp& other) const
{
    return Compare(*this, other) != 0;
}


bool StringCaseInsensitiveAcp::operator <(const StringCaseInsensitiveAcp& other) const
{
    return Compare(*this, other) < 0;
}


bool StringCaseInsensitiveAcp::operator <=(const StringCaseInsensitiveAcp& other) const
{
    return Compare(*this, other) <= 0;
}


bool StringCaseInsensitiveAcp::operator >(const StringCaseInsensitiveAcp& other) const
{
    return Compare(*this, other) > 0;
}


bool StringCaseInsensitiveAcp::operator >=(const StringCaseInsensitiveAcp& other) const
{
    return Compare(*this, other) >= 0;
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
    return m_ptr ? m_ptr->Ptr : "";
}


size_t StringCaseInsensitiveAcp::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
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
