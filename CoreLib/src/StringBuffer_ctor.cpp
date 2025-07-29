#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Heap.h"
#include "hnrt/StringCommons.h"
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
        StrCpy(m_ptr, other.m_ptr, other.m_len);
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
            MemCpy(m_ptr, psz, m_len);
        }
        else
        {
            MemCpy(m_ptr, psz, m_len - 1);
            set_Len(m_len - 1);
        }
    }
    else if (capacity < 0)
    {
        m_len = StrLen(psz);
        m_cap = m_len + 1;
        m_ptr = Allocate<WCHAR>(m_cap);
        MemCpy(m_ptr, psz, m_len + 1);
    }
}


StringBuffer::StringBuffer(const String& other)
    : m_ptr(Allocate<WCHAR>(other.Len + 1))
    , m_cap(other.Len + 1)
    , m_len(other.Len)
    , m_inc(1)
{
    MemCpy(m_ptr, other.Ptr, other.Len + 1);
}
