#include "pch.h"
#include "hnrt/StringUTF8.h"
#include "hnrt/RefMbs.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G - U T F 8   C L A S S
//
//////////////////////////////////////////////////////////////////////


StringUTF8::StringUTF8()
    : m_ptr(nullptr)
{
}


StringUTF8::StringUTF8(const StringUTF8& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


StringUTF8::StringUTF8(RefMbs* ptr)
    : m_ptr(ptr)
{
}


StringUTF8::StringUTF8(PCSTR psz)
    : m_ptr(psz ? new RefMbs(psz) : nullptr)
{
}


StringUTF8::StringUTF8(PCSTR psz, size_t cb)
    : m_ptr(psz ? new RefMbs(psz, cb) : nullptr)
{
}


StringUTF8::StringUTF8(PCWSTR psz)
    : m_ptr(psz ? new RefMbs(CP_UTF8, psz) : nullptr)
{
}


StringUTF8::StringUTF8(PCWSTR psz, size_t cb)
    : m_ptr(psz ? new RefMbs(CP_UTF8, psz, cb) : nullptr)
{
}


StringUTF8::~StringUTF8()
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


StringUTF8& StringUTF8::operator =(const StringUTF8& other)
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


StringUTF8& StringUTF8::operator +=(const StringUTF8& other)
{
    RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(Ptr, other.Ptr));
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


bool StringUTF8::operator ==(const StringUTF8& other) const
{
    return strcmp(*this, other) == 0;
}


bool StringUTF8::operator !=(const StringUTF8& other) const
{
    return strcmp(*this, other) != 0;
}


bool StringUTF8::operator <(const StringUTF8& other) const
{
    return strcmp(*this, other) < 0;
}


bool StringUTF8::operator <=(const StringUTF8& other) const
{
    return strcmp(*this, other) <= 0;
}


bool StringUTF8::operator >(const StringUTF8& other) const
{
    return strcmp(*this, other) > 0;
}


bool StringUTF8::operator >=(const StringUTF8& other) const
{
    return strcmp(*this, other) >= 0;
}


StringUTF8 StringUTF8::operator +(const StringUTF8& other) const
{
    return StringUTF8(new RefMbs(Ptr, other.Ptr));
}


PCSTR StringUTF8::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : "";
}


size_t StringUTF8::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
}
