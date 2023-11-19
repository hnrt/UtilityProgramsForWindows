#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"
#include "hnrt/String.h"


using namespace hnrt;


StringBuffer::StringBuffer(const StringBuffer& other)
    : m_ptr(other.m_cap ? Allocate<WCHAR>(other.m_cap) : nullptr)
    , m_cap(other.m_cap)
    , m_len(other.m_len)
{
    if (m_ptr)
    {
        wmemcpy_s(m_ptr, m_cap, other.m_ptr, other.m_len + 1);
    }
}


StringBuffer::StringBuffer(size_t capacity)
    : m_ptr(capacity ? Allocate<WCHAR>(capacity) : nullptr)
    , m_cap(capacity)
    , m_len(0)
{
    if (m_ptr)
    {
        m_ptr[0] = L'\0';
    }
}


StringBuffer::StringBuffer(INT_PTR capacity, PCWSTR psz)
    : m_ptr(nullptr)
    , m_cap(0)
    , m_len(0)
{
    if (capacity > 0)
    {
        m_ptr = Allocate<WCHAR>(capacity);
        m_cap = capacity;
        m_len = wcsnlen(psz, capacity);
        if (m_len < m_cap)
        {
            wmemcpy_s(m_ptr, m_cap, psz, m_len + 1);
        }
        else
        {
            wmemcpy_s(m_ptr, m_cap, psz, m_len);
            set_Len(m_len - 1);
        }
    }
    else if (capacity < 0)
    {
        m_len = wcslen(psz);
        m_cap = m_len + 1;
        m_ptr = Allocate<WCHAR>(m_cap);
        wmemcpy_s(m_ptr, m_cap, psz, m_len + 1);
    }
}


StringBuffer::StringBuffer(const String& other)
    : m_ptr(Allocate<WCHAR>(other.Len + 1))
    , m_cap(other.Len + 1)
    , m_len(other.Len)
{
    wmemcpy_s(m_ptr, m_cap, other, other.Len + 1);
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


StringBuffer& StringBuffer::Resize(size_t capacity)
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


StringBuffer& StringBuffer::Assign(PCWSTR psz, INT_PTR cch)
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
        wmemcpy_s(m_ptr + m_len, m_cap - m_len, other, other.Len + 1);
        m_len += other.Len;
    }
    return *this;
}


StringBuffer& StringBuffer::Append(const String& other)
{
    if (other.Len)
    {
        CheckCapacity(other.Len);
        wmemcpy_s(m_ptr + m_len, m_cap - m_len, other, other.Len + 1);
        m_len += other.Len;
    }
    return *this;
}


StringBuffer& StringBuffer::Append(PCWSTR psz, INT_PTR cch)
{
    if (cch)
    {
        if (cch > 0)
        {
            if (psz)
            {
                size_t cchActual = wcsnlen(psz, cch);
                CheckCapacity(cch);
                wmemcpy_s(m_ptr + m_len, m_cap - m_len, psz, cchActual);
                m_len += cchActual;
                m_ptr[m_len] = L'\0';
            }
            else
            {
                CheckCapacity(cch);
            }
        }
        else if (psz)
        {
            size_t cchActual = wcslen(psz);
            CheckCapacity(cchActual);
            wmemcpy_s(m_ptr + m_len, m_cap - m_len, psz, cchActual + 1);
            m_len += cchActual;
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
    va_list argList2;
    va_copy(argList2, argList);
    INT_PTR cch = _vscwprintf(pszFormat, argList2);
    va_end(argList2);
    if (cch < 0)
    {
        throw Exception(L"StringBuffer::VaAppendFormat failed.");
    }
    CheckCapacity(cch);
    _vsnwprintf_s(m_ptr + m_len, m_cap - m_len, _TRUNCATE, pszFormat, argList);
    m_len += cch;
    return *this;
}


void StringBuffer::set_Len(size_t value)
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


void StringBuffer::CheckCapacity(size_t delta)
{
    if (m_len + delta + 1 > m_cap)
    {
        size_t capacity = m_len + delta + MAX_PATH;
        m_ptr = Allocate(m_ptr, capacity);
        m_cap = capacity;
    }
}
