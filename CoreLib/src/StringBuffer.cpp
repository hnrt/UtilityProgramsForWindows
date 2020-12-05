#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Exception.h"
#include "hnrt/Heap.h"


using namespace hnrt;


StringBuffer::~StringBuffer()
{
    free(m_ptr);
}


StringBuffer::StringBuffer(size_t capacity)
    : m_ptr(Allocate<WCHAR>(capacity))
    , m_cap(capacity)
    , m_len(0)
{
}


void StringBuffer::set_Len(size_t value)
{
    if (m_len >= value)
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
    else
    {
        throw Exception(L"StringBuffer::set_Len: Invalid operation: Given %zu is greater than the length, %zu.", value, m_len);
    }
}


PWCHAR StringBuffer::Resize(size_t capacity)
{
    if (!capacity)
    {
        Deallocate(m_ptr);
        m_len = 0;
    }
    else if (capacity > m_cap)
    {
        m_ptr = Allocate(m_ptr, capacity);
    }
    else if (m_len >= capacity)
    {
        m_len = capacity - 1;
        m_ptr[m_len] = L'\0';
    }
    m_cap = capacity;
    return m_ptr;
}


PWCHAR StringBuffer::Detach()
{
    m_cap = 0;
    return ::Detach(m_ptr);
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
    int cch = _vscwprintf(pszFormat, argList);
    va_end(argList2);
    if (cch < 0)
    {
        throw Exception(L"StringBuffer::VaAppendFormat failed.");
    }
    if (m_len + cch + 1 > m_cap)
    {
        size_t capacity = m_len + cch + MAX_PATH;
        m_ptr = Allocate(m_ptr, capacity);
        m_cap = capacity;
    }
    _vsnwprintf_s(m_ptr + m_len, m_cap - m_len, _TRUNCATE, pszFormat, argList);
    m_len += cch;
    return *this;
}
