#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Heap.h"


using namespace hnrt;


void StringBuffer::CheckCapacity(SIZE_T delta)
{
    if (m_len + delta + 1 > m_cap)
    {
        SIZE_T capacity = m_len + delta + m_inc;
        m_ptr = Allocate(m_ptr, capacity);
        m_cap = capacity;
    }
}
