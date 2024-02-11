#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Heap.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Exception.h"
#include "hnrt/String.h"


using namespace hnrt;


StringBuffer::StringBuffer(const StringBuffer& other)
    : m_ptr(other.m_cap ? Allocate<WCHAR>(other.m_cap) : nullptr)
    , m_cap(other.m_cap)
    , m_len(other.m_len)
    , m_inc(1)
{
    if (m_ptr)
    {
        StrCopy(m_ptr, other.m_ptr, other.m_len);
    }
}


StringBuffer::StringBuffer(SIZE_T capacity)
    : m_ptr(capacity ? Allocate<WCHAR>(capacity) : nullptr)
    , m_cap(capacity)
    , m_len(0)
    , m_inc(capacity ? MAX_PATH : 1)
{
    if (m_ptr)
    {
        m_ptr[0] = L'\0';
    }
}


StringBuffer::StringBuffer(SSIZE_T capacity, PCWSTR psz)
    : m_ptr(nullptr)
    , m_cap(0)
    , m_len(0)
    , m_inc(capacity > 0 ? MAX_PATH : 1)
{
    if (capacity > 0)
    {
        m_ptr = Allocate<WCHAR>(capacity);
        m_cap = capacity;
        m_len = StrLen(psz, capacity);
        if (m_len < m_cap)
        {
            StrCopy(m_ptr, psz, m_len);
        }
        else
        {
            StrCopy(m_ptr, psz, m_len);
            set_Len(m_len - 1);
        }
    }
    else if (capacity < 0)
    {
        m_len = wcslen(psz);
        m_cap = m_len + 1;
        m_ptr = Allocate<WCHAR>(m_cap);
        StrCopy(m_ptr, psz, m_len);
    }
}


StringBuffer::StringBuffer(const String& other)
    : m_ptr(Allocate<WCHAR>(other.Len + 1))
    , m_cap(other.Len + 1)
    , m_len(other.Len)
    , m_inc(1)
{
    StrCopy(m_ptr, other.Ptr, other.Len);
}


StringBuffer::~StringBuffer()
{
    free(m_ptr);
}


PWCHAR StringBuffer::Detach()
{
    m_cap = m_len = 0;
    return ::Detach(m_ptr);
}


StringBuffer& StringBuffer::Resize(SIZE_T capacity)
{
    if (!capacity)
    {
        Deallocate(m_ptr);
        m_cap = 0;
        m_len = 0;
    }
    else if (capacity > m_cap)
    {
        m_ptr = Allocate(m_ptr, capacity);
        m_cap = capacity;
    }
    else if (m_len >= capacity)
    {
        m_cap = capacity;
        set_Len(capacity - 1);
    }
    return *this;
}


StringBuffer& StringBuffer::Assign(const StringBuffer& other)
{
    if (m_ptr)
    {
        m_len = 0;
        m_ptr[0] = L'\0';
    }
    return Append(other);
}


StringBuffer& StringBuffer::Assign(const String& other)
{
    if (m_ptr)
    {
        m_len = 0;
        m_ptr[0] = L'\0';
    }
    return Append(other);
}


StringBuffer& StringBuffer::Assign(PCWSTR psz, SSIZE_T cch)
{
    if (m_ptr)
    {
        m_len = 0;
        m_ptr[0] = L'\0';
    }
    return Append(psz, cch);
}


StringBuffer& StringBuffer::Format(PCWSTR pszFormat, ...)
{
    if (m_ptr)
    {
        m_len = 0;
        m_ptr[0] = L'\0';
    }
    va_list argList;
    va_start(argList, pszFormat);
    VaAppendFormat(pszFormat, argList);
    va_end(argList);
    return *this;
}


StringBuffer& StringBuffer::VaFormat(PCWSTR pszFormat, va_list argList)
{
    if (m_ptr)
    {
        m_len = 0;
        m_ptr[0] = L'\0';
    }
    return VaAppendFormat(pszFormat, argList);
}


StringBuffer& StringBuffer::Append(const StringBuffer& other)
{
    if (other.Len)
    {
        CheckCapacity(other.Len);
        m_len += StrCopy(m_ptr + m_len, other.Ptr, other.Len);
    }
    return *this;
}


StringBuffer& StringBuffer::Append(const String& other)
{
    if (other.Len)
    {
        CheckCapacity(other.Len);
        m_len += StrCopy(m_ptr + m_len, other.Ptr, other.Len);
    }
    return *this;
}


StringBuffer& StringBuffer::Append(PCWSTR psz, SSIZE_T cch)
{
    if (cch)
    {
        if (cch > 0)
        {
            if (psz)
            {
                CheckCapacity(cch);
                m_len += StrCopy(m_ptr + m_len, psz, cch);
            }
            else
            {
                CheckCapacity(cch);
            }
        }
        else if (psz)
        {
            cch = StrLen(psz);
            CheckCapacity(cch);
            m_len += StrCopy(m_ptr + m_len, psz, cch);
        }
    }
    return *this;
}


StringBuffer& StringBuffer::AppendFormat(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    VaAppendFormat(pszFormat, argList);
    va_end(argList);
    return *this;
}


StringBuffer& StringBuffer::VaAppendFormat(PCWSTR pszFormat, va_list argList)
{
    CheckCapacity(VaStrFmtLen(pszFormat, argList));
    m_len += hnrt::VaStrFmt(m_ptr + m_len, m_cap - m_len, pszFormat, argList);
    return *this;
}


StringBuffer& StringBuffer::Replace(WCHAR c1, WCHAR c2, SIZE_T offset, int count, SIZE_T* pOffset)
{
    if (count == 0)
    {
        if (pOffset)
        {
            *pOffset = offset;
        }
        return *this;
    }
    if (count < 0)
    {
        count = INT_MAX;
    }
    PWCHAR pCur = m_ptr + offset;
    PWCHAR pEnd = m_ptr + m_len;
    int index;
    while (pCur < pEnd && (index = IndexOf(pCur, c1, pEnd - pCur)) >= 0)
    {
        pCur[index] = c2;
        if (--count)
        {
            pCur += index + 1;
        }
        else
        {
            break;
        }
    }
    if (pOffset)
    {
        *pOffset = (pCur < pEnd) ? (pCur - m_ptr) : m_len;
    }
    return *this;
}


void StringBuffer::set_Len(SIZE_T value)
{
    if (m_len > value)
    {
        m_len = value;
        if (m_cap)
        {
            if (IS_LOW_SURROGATE(m_ptr[m_len]))
            {
                if (m_len)
                {
                    m_len--;
                }
            }
            m_ptr[m_len] = L'\0';
        }
    }
    else if (m_len < value)
    {
        throw Exception(L"StringBuffer::set_Len: Invalid operation: Given %zu is greater than the length, %zu.", value, m_len);
    }
}


void StringBuffer::CheckCapacity(SIZE_T delta)
{
    if (m_len + delta + 1 > m_cap)
    {
        SIZE_T capacity = m_len + delta + m_inc;
        m_ptr = Allocate(m_ptr, capacity);
        m_cap = capacity;
    }
}
