#include "pch.h"
#include "hnrt/StringCollection.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"
#include "hnrt/String.h"


using namespace hnrt;


StringCollection::StringCollection()
    : m_pArray(nullptr)
    , m_Capacity(0)
    , m_Count(0)
{
}


StringCollection::StringCollection(const StringCollection& src)
    : m_pArray(nullptr)
    , m_Capacity(0)
    , m_Count(0)
{
    if (src.m_pArray)
    {
        m_pArray = ZeroAllocate<PCWSTR>(src.m_Capacity);
        m_Capacity = src.m_Capacity;
        memcpy_s(m_pArray, m_Capacity * sizeof(PCWSTR), src.m_pArray, src.m_Count * sizeof(PCWSTR));
        m_Count = src.m_Count;
    }
}


StringCollection::~StringCollection()
{
    free(m_pArray);
}


StringCollection& StringCollection::operator =(const StringCollection& src)
{
    if (src.m_pArray)
    {
        m_pArray = Allocate(m_pArray, src.m_Capacity);
        m_Capacity = src.m_Capacity;
        memcpy_s(m_pArray, m_Capacity * sizeof(PCWSTR), src.m_pArray, src.m_Count * sizeof(PCWSTR));
        m_Count = src.m_Count;
        memset(m_pArray + m_Count, 0, (m_Capacity - m_Count) * sizeof(PCWSTR));
    }
    else
    {
        Deallocate(m_pArray);
        m_Capacity = 0;
        m_Count = 0;
    }
    return *this;

}


size_t StringCollection::get_Capacity() const
{
    return m_Capacity;
}


void StringCollection::set_Capacity(size_t value)
{
    if (value != m_Capacity)
    {
        if (value >= m_Count)
        {
            if (value)
            {
                m_pArray = Allocate(m_pArray, value);
                if (value > m_Capacity)
                {
                    memset(m_pArray + m_Capacity, 0, (value - m_Capacity) * sizeof(PCWSTR));
                }
                m_Capacity = value;
            }
            else
            {
                Deallocate(m_pArray);
                m_Capacity = 0;
            }
        }
    }
}


size_t StringCollection::get_Count() const
{
    return m_Count;
}


PCWSTR StringCollection::operator [](size_t index) const
{
    if (m_Count <= index)
    {
        throw Exception(L"StringCollection::operator []: Index out of range.");
    }
    return m_pArray[index];
}


StringCollection& StringCollection::Clear()
{
    m_Count = 0;
    return *this;
}


StringCollection& StringCollection::Add(PCWSTR psz, size_t cch)
{
    if (m_Count + 1 > m_Capacity)
    {
        set_Capacity(m_Capacity + (m_Capacity < 16 ? 16 : m_Capacity < 65536 ? m_Capacity : 65536));
    }
    m_pArray[m_Count++] = String::Copy(psz, cch);
    return *this;
}


StringCollection& StringCollection::RemoveAt(size_t index)
{
    if (index < m_Count)
    {
        m_Count--;
        while (index < m_Count)
        {
            m_pArray[index] = m_pArray[index + 1];
            index++;
        }
        m_pArray[index] = nullptr;
    }
    return *this;
}


StringCollection& StringCollection::Split(PCWSTR psz, WCHAR sep, bool bAll)
{
    PCWSTR pStop;
    while ((pStop = wcschr(psz, sep)))
    {
        size_t len = pStop - psz;
        if (bAll || len)
        {
            Add(psz, len);
        }
        psz = pStop + 1;
    }
    if (bAll || *psz)
    {
        Add(psz);
    }
    return *this;
}
