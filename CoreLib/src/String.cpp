#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/RefStr.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G   C L A S S
//
//////////////////////////////////////////////////////////////////////


const String String::Empty = String();


String::String()
    : m_ptr(nullptr)
{
}


String::String(const String& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


String::String(PCWSTR psz, INT_PTR cch)
    : m_ptr(psz && cch ? new RefStr(psz, cch) : nullptr)
{
}


String::String(PCWSTR pszFormat, va_list argList)
    : m_ptr(new RefStr(pszFormat, argList))
{
}


String::String(StringOptions option, PCWSTR psz, ...)
    : m_ptr(nullptr)
{
    va_list argList;
    va_start(argList, psz);
    switch (option)
    {
    case PRINTF:
        m_ptr = new RefStr(psz, argList);
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
        m_ptr = new RefStr(IMMEDIATE_TEXT, StringUtils::VaConcat(option, psz, argList));
        break;
    case UPPERCASE:
    case LOWERCASE:
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
    case IMMEDIATE_TEXT:
    case STATIC_TEXT:
        m_ptr = new RefStr(option, psz);
        break;
    default:
        throw Exception(L"String::ctor: Bad option.");
    }
    va_end(argList);
}


String::String(PCWSTR psz1, PCWSTR psz2)
    : m_ptr(new RefStr(psz1, psz2))
{
}


String::String(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3)
    : m_ptr(new RefStr(psz1, psz2, psz3))
{
}


String::String(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4)
    : m_ptr(new RefStr(psz1, psz2, psz3, psz4))
{
}


String::String(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5)
    : m_ptr(new RefStr(psz1, psz2, psz3, psz4, psz5))
{
}


String::String(UINT cp, PCSTR psz, INT_PTR cb)
    : m_ptr(psz && cb ? new RefStr(cp, psz, cb) : nullptr)
{
}


String::~String()
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


String& String::ZeroFill()
{
    if (m_ptr)
    {
        m_ptr->ZeroFill();
    }
    return *this;
}


String& String::Uppercase()
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
            RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(UPPERCASE, Ptr));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


String& String::Lowercase()
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
            RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(LOWERCASE, Ptr));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


String& String::Trim(StringOptions option)
{
    if (Len)
    {
        if (m_ptr->RefCnt == 1)
        {
            m_ptr->Exchange(StringUtils::Trim(option, m_ptr->Detach()));
        }
        else
        {
            RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(option, Ptr));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


String& String::Assign(const String& other)
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


String& String::Assign(PCWSTR psz, INT_PTR cch)
{
    if (psz && cch)
    {
        if (m_ptr && m_ptr->RefCnt == 1)
        {
            m_ptr->Assign(psz, cch);
        }
        else
        {
            RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(psz, cch));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    else
    {
        RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, nullptr);
        if (ptr)
        {
            ptr->Release();
        }
    }
    return *this;
}


String& String::Append(const String& other)
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


String& String::Append(PCWSTR psz, INT_PTR cch)
{
    if (psz && cch)
    {
        if (m_ptr && m_ptr->RefCnt == 1)
        {
            m_ptr->Append(psz, cch);
        }
        else
        {
            cch = cch > 0 ? wcsnlen(psz, cch) : wcslen(psz);
            PWSTR psz2 = Allocate<WCHAR>(Len + cch + 1);
            wmemcpy_s(psz2, Len, Ptr, Len);
            wmemcpy_s(psz2 + Len, cch, psz, cch);
            psz2[Len + cch] = L'\0';
            RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(IMMEDIATE_TEXT, psz2));
            if (ptr)
            {
                ptr->Release();
            }
        }
    }
    return *this;
}


String& String::Format(PCWSTR psz, ...)
{
    va_list argList;
    va_start(argList, psz);
    if (m_ptr && m_ptr->RefCnt == 1)
    {
        m_ptr->Exchange(::VaAppendFormat(m_ptr->Detach(), 0, psz, argList));
    }
    else
    {
        RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(psz, argList));
        if (ptr)
        {
            ptr->Release();
        }
    }
    va_end(argList);
    return *this;
}


String& String::VaFormat(PCWSTR psz, va_list argList)
{
    if (m_ptr && m_ptr->RefCnt == 1)
    {
        m_ptr->Exchange(::VaAppendFormat(m_ptr->Detach(), 0, psz, argList));
    }
    else
    {
        RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(psz, argList));
        if (ptr)
        {
            ptr->Release();
        }
    }
    return *this;
}


String& String::AppendFormat(PCWSTR psz, ...)
{
    va_list argList;
    va_start(argList, psz);
    if (m_ptr && m_ptr->RefCnt == 1)
    {
        INT_PTR cch = m_ptr->Len;
        m_ptr->Exchange(::VaAppendFormat(m_ptr->Detach(), cch, psz, argList));
    }
    else
    {
        RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(IMMEDIATE_TEXT, VaConcatFormat(Ptr, Len, psz, argList)));
        if (ptr)
        {
            ptr->Release();
        }
    }
    va_end(argList);
    return *this;
}


String& String::VaAppendFormat(PCWSTR psz, va_list argList)
{
    if (m_ptr && m_ptr->RefCnt == 1)
    {
        INT_PTR cch = m_ptr->Len;
        m_ptr->Exchange(::VaAppendFormat(m_ptr->Detach(), cch, psz, argList));
    }
    else
    {
        RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(IMMEDIATE_TEXT, VaConcatFormat(Ptr, Len, psz, argList)));
        if (ptr)
        {
            ptr->Release();
        }
    }
    return *this;
}


int String::IndexOf(WCHAR c, INT_PTR fromIndex)
{
    if (m_ptr && static_cast<size_t>(fromIndex) < m_ptr->Len)
    {
        const WCHAR* pCur = wmemchr(m_ptr->Ptr + fromIndex, c, m_ptr->Len - fromIndex);
        return pCur ? static_cast<int>(pCur - m_ptr->Ptr) : -1;
    }
    return -1;
}


bool String::operator ==(const String& other) const
{
    return Compare(*this, other) == 0;
}


bool String::operator !=(const String& other) const
{
    return Compare(*this, other) != 0;
}


bool String::operator <(const String& other) const
{
    return Compare(*this, other) < 0;
}


bool String::operator <=(const String& other) const
{
    return Compare(*this, other) <= 0;
}


bool String::operator >(const String& other) const
{
    return Compare(*this, other) > 0;
}


bool String::operator >=(const String& other) const
{
    return Compare(*this, other) >= 0;
}


String String::operator +(const String& other) const
{
    return String(Ptr, other.Ptr);
}


PCWSTR String::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : L"";
}


size_t String::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
}


int String::Compare(PCWSTR psz1, INT_PTR cch1, PCWSTR psz2, INT_PTR cch2)
{
    switch (CompareStringW(LOCALE_INVARIANT, 0, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
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
