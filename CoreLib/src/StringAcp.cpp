#include "pch.h"
#include "hnrt/StringAcp.h"
#include "hnrt/RefMbs.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G - A C P   C L A S S
//
//////////////////////////////////////////////////////////////////////


StringAcp::StringAcp()
    : m_ptr(nullptr)
{
}


StringAcp::StringAcp(const StringAcp& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


StringAcp::StringAcp(RefMbs* ptr)
    : m_ptr(ptr)
{
}


StringAcp::StringAcp(PCSTR psz)
    : m_ptr(psz ? new RefMbs(psz) : nullptr)
{
}


StringAcp::StringAcp(PCSTR psz, size_t cb)
    : m_ptr(psz ? new RefMbs(psz, cb) : nullptr)
{
}


StringAcp::StringAcp(PCWSTR psz)
    : m_ptr(psz ? new RefMbs(CP_ACP, psz) : nullptr)
{
}


StringAcp::StringAcp(PCWSTR psz, size_t cb)
    : m_ptr(psz ? new RefMbs(CP_ACP, psz, cb) : nullptr)
{
}


StringAcp::~StringAcp()
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


StringAcp& StringAcp::operator =(const StringAcp& other)
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


StringAcp& StringAcp::operator +=(const StringAcp& other)
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(Ptr, other.Ptr));
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


bool StringAcp::operator ==(const StringAcp& other) const
{
    return Compare(*this, other) == 0;
}


bool StringAcp::operator !=(const StringAcp& other) const
{
    return Compare(*this, other) != 0;
}


bool StringAcp::operator <(const StringAcp& other) const
{
    return Compare(*this, other) < 0;
}


bool StringAcp::operator <=(const StringAcp& other) const
{
    return Compare(*this, other) <= 0;
}


bool StringAcp::operator >(const StringAcp& other) const
{
    return Compare(*this, other) > 0;
}


bool StringAcp::operator >=(const StringAcp& other) const
{
    return Compare(*this, other) >= 0;
}


StringAcp StringAcp::operator +(const StringAcp& other) const
{
    return StringAcp(new RefMbs(Ptr, other.Ptr));
}


PCSTR StringAcp::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : "";
}


size_t StringAcp::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
}


int StringAcp::Compare(PCSTR psz1, INT_PTR cb1, PCSTR psz2, INT_PTR cb2)
{
    switch (CompareStringA(LOCALE_INVARIANT, 0, psz1, static_cast<int>(cb1), psz2, static_cast<int>(cb2)))
    {
    case CSTR_EQUAL:
        return 0;
    case CSTR_LESS_THAN:
        return -1;
    case CSTR_GREATER_THAN:
        return 1;
    default:
        throw Exception(L"String::Compare failed.");
    }
}


int StringAcp::CaseCompare(PCSTR psz1, INT_PTR cb1, PCSTR psz2, INT_PTR cb2)
{
    switch (CompareStringA(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, static_cast<int>(cb1), psz2, static_cast<int>(cb2)))
    {
    case CSTR_EQUAL:
        return 0;
    case CSTR_LESS_THAN:
        return -1;
    case CSTR_GREATER_THAN:
        return 1;
    default:
        throw Exception(L"String::CaseCompare failed.");
    }
}
