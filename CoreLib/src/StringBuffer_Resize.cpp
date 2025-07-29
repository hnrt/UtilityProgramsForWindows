#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/String.h"
#include "hnrt/Heap.h"


using namespace hnrt;


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
