#include "pch.h"
#include "hnrt/StringAcp.h"
#include "hnrt/RefMbs.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G - A C P   C L A S S
//
//////////////////////////////////////////////////////////////////////


const StringAcp StringAcp::Empty = StringAcp();


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


StringAcp::StringAcp(PCSTR psz, INT_PTR cb)
    : m_ptr(psz && cb ? new RefMbs(psz, cb) : nullptr)
{
}


StringAcp::StringAcp(PCSTR psz, va_list argList)
    : m_ptr(new RefMbs(psz, argList))
{
}


StringAcp::StringAcp(StringOptions option, PCSTR psz, ...)
    : m_ptr(nullptr)
{
    va_list argList;
    va_start(argList, psz);
    switch (option)
    {
    case PRINTF:
        m_ptr = new RefMbs(psz, argList);
        break;
    case CONCAT:
    case CONCAT2:
    case CONCAT3:
    case CONCAT4:
    case CONCAT5:
    case CONCAT6:
    case CONCAT7:
    case CONCAT8:
    case CONCAT9:
    {
        m_ptr = new RefMbs(IMMEDIATE_TEXT, StringUtils::VaConcat(option, psz, argList, Allocate<CHAR>(StringUtils::VaConcatSize(option, psz, argList))));
        break;
    }
    case UPPERCASE:
    case LOWERCASE:
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
    case IMMEDIATE_TEXT:
    case STATIC_TEXT:
        m_ptr = new RefMbs(option, psz);
        break;
    default:
        throw Exception(L"StringAcp::ctor: Bad option.");
    }
    va_end(argList);
}


StringAcp::StringAcp(PCSTR psz1, PCSTR psz2)
    : m_ptr(new RefMbs(psz1, psz2))
{
}


StringAcp::StringAcp(PCSTR psz1, PCSTR psz2, PCSTR psz3)
    : m_ptr(new RefMbs(psz1, psz2, psz3))
{
}


StringAcp::StringAcp(PCSTR psz1, PCSTR psz2, PCSTR psz3, PCSTR psz4)
    : m_ptr(new RefMbs(psz1, psz2, psz3, psz4))
{
}


StringAcp::StringAcp(PCSTR psz1, PCSTR psz2, PCSTR psz3, PCSTR psz4, PCSTR psz5)
    : m_ptr(new RefMbs(psz1, psz2, psz3, psz4, psz5))
{
}


StringAcp::StringAcp(PCWSTR psz, INT_PTR cch)
    : m_ptr(psz && cch ? new RefMbs(CP_ACP, psz, cch) : nullptr)
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


StringAcp& StringAcp::ZeroFill()
{
    if (m_ptr)
    {
        m_ptr->ZeroFill();
    }
    return *this;
}


StringAcp& StringAcp::Uppercase()
{
    if (Len)
    {
        if (m_ptr->RefCnt == 1)
        {
            size_t size = Len + 1;
            m_ptr->Exchange(StringUtils::Uppercase(m_ptr->Detach(), size));
        }
        else
        {
            RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(UPPERCASE, Ptr));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


StringAcp& StringAcp::Lowercase()
{
    if (Len)
    {
        if (m_ptr->RefCnt == 1)
        {
            size_t size = Len + 1;
            m_ptr->Exchange(StringUtils::Lowercase(m_ptr->Detach(), size));
        }
        else
        {
            RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(LOWERCASE, Ptr));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


StringAcp& StringAcp::Trim(StringOptions option)
{
    if (Len)
    {
        if (m_ptr->RefCnt == 1)
        {
            m_ptr->Exchange(StringUtils::Trim(option, m_ptr->Detach()));
        }
        else
        {
            RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(option, Ptr));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


StringAcp& StringAcp::Assign(const StringAcp& other)
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


StringAcp& StringAcp::Assign(PCSTR psz, INT_PTR cb)
{
    if (psz && cb)
    {
        if (m_ptr && m_ptr->RefCnt == 1)
        {
            m_ptr->Assign(psz, cb);
        }
        else
        {
            RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(psz, cb));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    else
    {
        RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, nullptr);
        if (ptr)
        {
            ptr->Release();
        }
    }
    return *this;
}


StringAcp& StringAcp::Append(const StringAcp& other)
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


StringAcp& StringAcp::Append(PCSTR psz, INT_PTR cb)
{
    if (psz && cb)
    {
        if (m_ptr && m_ptr->RefCnt == 1)
        {
            m_ptr->Append(psz, cb);
        }
        else
        {
            cb = cb > 0 ? strnlen(psz, cb) : strlen(psz);
            PSTR psz2 = Allocate<CHAR>(Len + cb + 1);
            memcpy_s(psz2, Len, Ptr, Len);
            memcpy_s(psz2 + Len, cb, psz, cb);
            psz2[Len + cb] = '\0';
            RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(IMMEDIATE_TEXT, psz2));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


StringAcp& StringAcp::Format(PCSTR psz, ...)
{
    va_list argList;
    va_start(argList, psz);
    if (m_ptr && m_ptr->RefCnt == 1)
    {
        m_ptr->Exchange(::VaAppendFormat(m_ptr->Detach(), 0, psz, argList));
    }
    else
    {
        RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(psz, argList));
        if (ptr)
        {
            ptr->Release();
        }
    }
    va_end(argList);
    return *this;
}


StringAcp& StringAcp::VaFormat(PCSTR psz, va_list argList)
{
    if (m_ptr && m_ptr->RefCnt == 1)
    {
        m_ptr->Exchange(::VaAppendFormat(m_ptr->Detach(), 0, psz, argList));
    }
    else
    {
        RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(psz, argList));
        if (ptr)
        {
            ptr->Release();
        }
    }
    return *this;
}


StringAcp& StringAcp::AppendFormat(PCSTR psz, ...)
{
    va_list argList;
    va_start(argList, psz);
    if (m_ptr && m_ptr->RefCnt == 1)
    {
        INT_PTR cb = m_ptr->Len;
        m_ptr->Exchange(::VaAppendFormat(m_ptr->Detach(), cb, psz, argList));
    }
    else
    {
        RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(IMMEDIATE_TEXT, VaConcatFormat(Ptr, Len, psz, argList)));
        if (ptr)
        {
            ptr->Release();
        }
    }
    va_end(argList);
    return *this;
}


StringAcp& StringAcp::VaAppendFormat(PCSTR psz, va_list argList)
{
    if (m_ptr && m_ptr->RefCnt == 1)
    {
        INT_PTR cb = m_ptr->Len;
        m_ptr->Exchange(::VaAppendFormat(m_ptr->Detach(), cb, psz, argList));
    }
    else
    {
        RefMbs* ptr = Interlocked<RefMbs*>::ExchangePointer(&m_ptr, new RefMbs(IMMEDIATE_TEXT, VaConcatFormat(Ptr, Len, psz, argList)));
        if (ptr)
        {
            ptr->Release();
        }
    }
    return *this;
}


int StringAcp::IndexOf(CHAR c, INT_PTR fromIndex)
{
    if (m_ptr && static_cast<size_t>(fromIndex) < m_ptr->Len)
    {
        const CHAR* pCur = reinterpret_cast<const CHAR*>(memchr(m_ptr->Ptr + fromIndex, c, m_ptr->Len - fromIndex));
        return pCur ? static_cast<int>(pCur - m_ptr->Ptr) : -1;
    }
    return -1;
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
    return StringAcp(Ptr, other.Ptr);
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
