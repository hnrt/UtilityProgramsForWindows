#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/RefStr.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Heap.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/StringStore.h"
#include "hnrt/Exception.h"
#include "hnrt/Buffer.h"
#include <exception>


using namespace hnrt;


String String::Format2(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    String s(pszFormat, argList);
    va_end(argList);
    return s;
}


String::String()
    : m_ptr(nullptr)
{
}


String::String(PCWSTR psz)
    : m_ptr(new RefStr(psz))
{
}


String::String(PCWSTR psz, size_t cch)
    : m_ptr(new RefStr(psz, cch))
{
}


String::String(PCWSTR pszFormat, va_list argList)
    : m_ptr(new RefStr(pszFormat, argList))
{
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


String::String(const String& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr)
    {
        m_ptr->AddRef();
    }
}


String::~String()
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, nullptr);
    if (ptr)
    {
        ptr->Release();
    }
}


String& String::operator =(const String& other)
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


bool String::operator ==(const String& other) const
{
    return wcscmp(Str, other.Str) == 0;
}


bool String::operator !=(const String& other) const
{
    return wcscmp(Str, other.Str) != 0;
}


bool String::operator <(const String& other) const
{
    return wcscmp(Str, other.Str) < 0;
}


bool String::operator <=(const String& other) const
{
    return wcscmp(Str, other.Str) <= 0;
}


bool String::operator >(const String& other) const
{
    return wcscmp(Str, other.Str) > 0;
}


bool String::operator >=(const String& other) const
{
    return wcscmp(Str, other.Str) >= 0;
}


String String::operator +(const String& other) const
{
    return String(Str, other.Str);
}


String& String::operator +=(const String& other)
{
    RefStr* ptr = Interlocked<RefStr*>::ExchangePointer(&m_ptr, new RefStr(Str, other.Str));
    if (ptr)
    {
        ptr->Release();
    }
    return *this;
}


String::operator PCWSTR() const
{
    return get_str();
}


PCWSTR String::get_ptr() const
{
    return m_ptr ? m_ptr->Ptr : nullptr;
}


PCWSTR String::get_str() const
{
    return m_ptr ? m_ptr->Str : L"";
}


size_t String::get_len() const
{
    return m_ptr ? m_ptr->Len : 0;
}


String hnrt::FormatString(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    String s(pszFormat, argList);
    va_end(argList);
    return s;
}


PCWSTR String::Copy(PCWSTR psz, size_t cch)
{
    if (!psz)
    {
        return L"";
    }
    else if (cch == static_cast<size_t>(-1))
    {
        return StringStore::Get(psz);
    }
    else if (cch < MAX_PATH)
    {
        WCHAR buf[MAX_PATH];
        wcsncpy_s(buf, psz, cch);
        return StringStore::Get(buf);
    }
    else
    {
        StringBuffer buf(cch + 1);
        wcsncpy_s(buf, buf.Cap, psz, cch);
        return StringStore::Set(buf);
    }
}


PCWSTR String::Append(PCWSTR psz1, PCWSTR psz2, size_t cch2)
{
    if (!psz1)
    {
        return Copy(psz2, cch2);
    }
    size_t cch1 = wcslen(psz1);
    if (cch2 == static_cast<size_t>(-1))
    {
        cch2 = wcslen(psz2);
    }
    if (cch1 + cch2 < MAX_PATH)
    {
        WCHAR buf[MAX_PATH];
        wcsncpy_s(buf, psz1, cch1);
        wcsncpy_s(buf + cch1, _countof(buf) - cch1, psz2, cch2);
        return StringStore::Get(buf);
    }
    else
    {
        StringBuffer buf(cch1 + cch2 + 1);
        wcsncpy_s(buf, buf.Cap, psz1, cch1);
        wcsncpy_s(buf + cch1, buf.Cap - cch1, psz2, cch2);
        return StringStore::Set(buf);
    }
}


PCWSTR String::Format(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    PCWSTR psz = VaFormat(pszFormat, argList);
    va_end(argList);
    return psz;
}


PCWSTR String::VaFormat(PCWSTR pszFormat, va_list argList)
{
    va_list argList2;
    va_copy(argList2, argList);
    int cch = _vscwprintf(pszFormat, argList);
    va_end(argList2);
    if (cch < 0)
    {
        throw Exception(L"String::VaFormat failed.");
    }
    else if (cch < MAX_PATH)
    {
        WCHAR buf[MAX_PATH];
        _vsnwprintf_s(buf, _TRUNCATE, pszFormat, argList);
        return StringStore::Get(buf);
    }
    else
    {
        StringBuffer buf(static_cast<size_t>(cch) + 1);
        _vsnwprintf_s(buf, buf.Cap, _TRUNCATE, pszFormat, argList);
        return StringStore::Set(buf);
    }
}


PCWSTR String::AppendFormat(PCWSTR psz, PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    psz = VaAppendFormat(psz, pszFormat, argList);
    va_end(argList);
    return psz;
}


PCWSTR String::VaAppendFormat(PCWSTR psz, PCWSTR pszFormat, va_list argList)
{
    size_t cch1 = wcslen(psz);
    va_list argList2;
    va_copy(argList2, argList);
    int cch2 = _vscwprintf(pszFormat, argList);
    va_end(argList2);
    if (cch2 < 0)
    {
        throw Exception(L"String::VaAppendFormat failed.");
    }
    else if (cch1 + cch2 < MAX_PATH)
    {
        WCHAR buf[MAX_PATH];
        wcsncpy_s(buf, psz, cch1);
        _vsnwprintf_s(buf + cch1, _countof(buf) - cch1, _TRUNCATE, pszFormat, argList);
        return StringStore::Get(buf);
    }
    else
    {
        StringBuffer buf(cch1 + cch2 + 1);
        wcsncpy_s(buf, buf.Cap, psz, cch1);
        _vsnwprintf_s(buf + cch1, buf.Cap - cch1, _TRUNCATE, pszFormat, argList);
        return StringStore::Set(buf);
    }
}


PCWSTR String::Trim(PCWSTR psz, size_t cch)
{
    if (!psz)
    {
        return L"";
    }
    else if (cch == static_cast<size_t>(-1))
    {
        cch = wcslen(psz);
    }
    PCWSTR pszEnd = psz + cch;
    while (true)
    {
        if (psz == pszEnd)
        {
            return L"";
        }
        else if (iswspace(psz[0]))
        {
            psz++;
        }
        else
        {
            // Found the first non-space character.
            break;
        }
    }
    // Repetitions will end at the non-space character.
    do
    {
        pszEnd--;
    }
    while (iswspace(pszEnd[0]));
    return Copy(psz, pszEnd + 1 - psz);
}


PCWSTR String::TrimHead(PCWSTR psz, size_t cch)
{
    if (!psz)
    {
        return L"";
    }
    else if (cch == static_cast<size_t>(-1))
    {
        cch = wcslen(psz);
    }
    PCWSTR pszEnd = psz + cch;
    while (true)
    {
        if (psz == pszEnd)
        {
            return L"";
        }
        else if (iswspace(psz[0]))
        {
            psz++;
        }
        else
        {
            // Found the first non-space character.
            return Copy(psz, pszEnd - psz);
        }
    }
}


PCWSTR String::TrimTail(PCWSTR psz, size_t cch)
{
    if (!psz)
    {
        return L"";
    }
    else if (cch == static_cast<size_t>(-1))
    {
        cch = wcslen(psz);
    }
    PCWSTR pszEnd = psz + cch;
    while (true)
    {
        if (psz == pszEnd)
        {
            return L"";
        }
        pszEnd--;
        if (!iswspace(pszEnd[0]))
        {
            // Found the last non-space character.
            return Copy(psz, pszEnd + 1 - psz);
        }
    }
}


int String::Compare(PCWSTR psz1, PCWSTR psz2, size_t cch2)
{
    switch (CompareStringW(LOCALE_INVARIANT, 0, psz1, -1, psz2, static_cast<int>(cch2)))
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


int String::CaseCompare(PCWSTR psz1, PCWSTR psz2, size_t cch2)
{
    switch (CompareStringW(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, -1, psz2, static_cast<int>(cch2)))
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


int String::Compare(PCWSTR psz1, size_t cch1, PCWSTR psz2, size_t cch2)
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


int String::CaseCompare(PCWSTR psz1, size_t cch1, PCWSTR psz2, size_t cch2)
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
        throw Exception(L"String::CaseCompare failed.");
    }
}


int String::Compare(PCSTR psz1, PCSTR psz2, size_t cch2)
{
    switch (CompareStringA(LOCALE_INVARIANT, 0, psz1, -1, psz2, static_cast<int>(cch2)))
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


int String::CaseCompare(PCSTR psz1, PCSTR psz2, size_t cch2)
{
    switch (CompareStringA(LOCALE_INVARIANT, LINGUISTIC_IGNORECASE, psz1, -1, psz2, static_cast<int>(cch2)))
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


int String::Compare(PCSTR psz1, size_t cch1, PCSTR psz2, size_t cch2)
{
    switch (CompareStringA(LOCALE_INVARIANT, 0, psz1, static_cast<int>(cch1), psz2, static_cast<int>(cch2)))
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


int String::CaseCompare(PCSTR psz1, size_t cch1, PCSTR psz2, size_t cch2)
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
        throw Exception(L"String::CaseCompare failed.");
    }
}


PCWSTR String::ToUcs(PCSTR psz, size_t cch)
{
    if (!psz || !*psz || !cch)
    {
        return L"";
    }
    int m = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, psz, static_cast<int>(cch), NULL, 0);
    if (!m)
    {
        throw Exception(L"String::ToUcs: Conversion failed.");
    }
    Buffer<WCHAR> buf(static_cast<size_t>(m) + 1);
    int n = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, psz, static_cast<int>(cch), buf, m + 1);
    if (m != n)
    {
        throw Exception(L"String::ToUcs: Conversion failed. Inconsistent results.");
    }
    buf[n] = L'\0';
    return StringStore::Get(buf);
}


PCSTR String::ToAcp(PCWSTR psz, size_t cch)
{
    if (!psz || !*psz || !cch)
    {
        return "";
    }
    int m = WideCharToMultiByte(CP_ACP, 0, psz, static_cast<int>(cch), NULL, 0, NULL, NULL);
    if (!m)
    {
        throw Exception(L"String::ToAcp: Conversion failed.");
    }
    Buffer<CHAR> buf(static_cast<size_t>(m) + 1);
    int n = WideCharToMultiByte(CP_ACP, 0, psz, static_cast<int>(cch), buf, m + 1, NULL, NULL);
    if (m != n)
    {
        throw Exception(L"String::ToAcp: Conversion failed. Inconsistent results.");
    }
    buf[n] = '\0';
    return StringStore::Get(buf);
}
