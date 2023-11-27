#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/RefStr.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


using namespace hnrt;


//////////////////////////////////////////////////////////////////////
//
//   S T R I N G   C L A S S
//
//////////////////////////////////////////////////////////////////////


const String String::Empty = String();


String::String()
    : m_psz(nullptr)
{
}


String::String(const String& other)
    : m_psz(other.m_psz)
{
    if (m_psz)
    {
        RefStr::Get(m_psz).AddRef();
    }
}


String::String(PCWSTR psz, INT_PTR cch)
    : m_psz(psz && cch ? RefStr::Create(psz, cch) : nullptr)
{
}


String::String(PCWSTR pszFormat, va_list argList)
    : m_psz(RefStr::Create(pszFormat, argList))
{
}


String::String(StringOptions option, PCWSTR psz, ...)
    : m_psz(nullptr)
{
    va_list argList;
    va_start(argList, psz);
    switch (option)
    {
    case PRINTF:
        m_psz = RefStr::Create(psz, argList);
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
        m_psz = RefStr::Create(option, psz, argList);
        break;
    case UPPERCASE:
        m_psz = RefStr::Create(UPPERCASE, psz);
        break;
    case LOWERCASE:
        m_psz = RefStr::Create(LOWERCASE, psz);
        break;
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
    {
        int start = 0;
        int end = 0;
        StringUtils::TrimScan(option, psz, start, end);
        int cch = end - start;
        m_psz = RefStr::Create(psz + start, cch);
        break;
    }
    default:
        throw Exception(L"String::ctor: Bad option.");
    }
    va_end(argList);
}


String::String(PCWSTR psz1, PCWSTR psz2)
    : m_psz(RefStr::Create(psz1, psz2))
{
}


String::String(UINT cp, PCSTR psz, INT_PTR cb)
    : m_psz(psz && cb ? RefStr::Create(cp, psz, cb) : nullptr)
{
}


String::~String()
{
    PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, nullptr);
    if (psz)
    {
        RefStr::Get(psz).Release();
    }
}


String& String::ZeroFill()
{
    if (m_psz)
    {
        RefStr::Get(m_psz).ZeroFill();
    }
    return *this;
}


String& String::Uppercase()
{
    if (Len)
    {
        PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, RefStr::Create(UPPERCASE, m_psz));
        if (psz)
        {
            RefStr::Get(psz).Release();
        }
    }
    return *this;
}


String& String::Lowercase()
{
    if (Len)
    {
        PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, RefStr::Create(LOWERCASE, m_psz));
        if (psz)
        {
            RefStr::Get(psz).Release();
        }
    }
    return *this;
}


String& String::Trim(StringOptions option)
{
    if (Len)
    {
        int start = 0;
        int end = 0;
        StringUtils::TrimScan(option, m_psz, start, end);
        int cch = end - start;
        PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, RefStr::Create(m_psz + start, cch));
        if (psz)
        {
            RefStr::Get(psz).Release();
        }
    }
    return *this;
}


String& String::Assign(const String& other)
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


String& String::Assign(PCWSTR psz, INT_PTR cch)
{
    if (psz && cch)
    {
        psz = InterlockedExchangePCWSTR(&m_psz, RefStr::Create(psz, cch));
        if (psz)
        {
            RefStr::Get(psz).Release();
        }
    }
    else
    {
        psz = InterlockedExchangePCWSTR(&m_psz, nullptr);
        if (psz)
        {
            RefStr::Get(psz).Release();
        }
    }
    return *this;
}


String& String::Append(const String& other)
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


String& String::Append(PCWSTR psz, INT_PTR cch)
{
    if (psz && cch)
    {
        psz = InterlockedExchangePCWSTR(&m_psz, RefStr::Create(m_psz, psz, cch));
        if (psz)
        {
            RefStr::Get(psz).Release();
        }
    }
    return *this;
}


String& String::Format(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, RefStr::Create(pszFormat, argList));
    if (psz)
    {
        RefStr::Get(psz).Release();
    }
    va_end(argList);
    return *this;
}


String& String::VaFormat(PCWSTR pszFormat, va_list argList)
{
    PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, RefStr::Create(pszFormat, argList));
    if (psz)
    {
        RefStr::Get(psz).Release();
    }
    return *this;
}


String& String::AppendFormat(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, m_psz ? RefStr::Create(m_psz, pszFormat, argList) : RefStr::Create(pszFormat, argList));
    if (psz)
    {
        RefStr::Get(psz).Release();
    }
    va_end(argList);
    return *this;
}


String& String::VaAppendFormat(PCWSTR pszFormat, va_list argList)
{
    PCWSTR psz = InterlockedExchangePCWSTR(&m_psz, m_psz ? RefStr::Create(m_psz, pszFormat, argList) : RefStr::Create(pszFormat, argList));
    if (psz)
    {
        RefStr::Get(psz).Release();
    }
    return *this;
}


int String::IndexOf(WCHAR c, INT_PTR fromIndex)
{
    if (m_psz && static_cast<size_t>(fromIndex) < Len)
    {
        const WCHAR* pCur = wmemchr(m_psz + fromIndex, c, Len - fromIndex);
        return pCur ? static_cast<int>(pCur - m_psz) : -1;
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
    return m_psz ? m_psz : L"";
}


size_t String::get_len() const
{
    return m_psz ? RefStr::Get(m_psz).Len : 0;
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
