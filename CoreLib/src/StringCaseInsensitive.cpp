#include "pch.h"
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/RefStr.h"
#include "hnrt/String.h"


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


StringCaseInsensitive::StringCaseInsensitive(const StringCaseInsensitive& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


StringCaseInsensitive::StringCaseInsensitive(PCWSTR psz, INT_PTR cch)
    : m_ptr(psz && cch ? new RefStr(psz, cch) : nullptr)
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


StringCaseInsensitive::~StringCaseInsensitive()
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
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


StringCaseInsensitive& StringCaseInsensitive::operator +=(const StringCaseInsensitive& other)
{
    if (other.Len)
    {
        if (m_ptr && m_ptr->RefCnt == 1)
        {
            m_ptr->Append(other.Ptr);
        }
        else
        {
            PWSTR psz2 = Allocate<WCHAR>(Len + other.Len + 1);
            wmemcpy_s(psz2, Len, Ptr, Len);
            wmemcpy_s(psz2 + Len, other.Len, other.Ptr, other.Len);
            psz2[Len + other.Len] = L'\0';
            RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(IMMEDIATE_TEXT, psz2));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


StringCaseInsensitive& StringCaseInsensitive::operator +=(const String& other)
{
    if (other.Len)
    {
        if (m_ptr && m_ptr->RefCnt == 1)
        {
            m_ptr->Append(other.Ptr);
        }
        else
        {
            PWSTR psz2 = Allocate<WCHAR>(Len + other.Len + 1);
            wmemcpy_s(psz2, Len, Ptr, Len);
            wmemcpy_s(psz2 + Len, other.Len, other.Ptr, other.Len);
            psz2[Len + other.Len] = L'\0';
            RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(IMMEDIATE_TEXT, psz2));
            if (ptr)
            {
                ptr->Release();
            }
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
    return m_ptr ? m_ptr->Ptr : L"";
}


size_t StringCaseInsensitive::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
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
