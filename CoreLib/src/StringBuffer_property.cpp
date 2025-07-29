#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/Exception.h"


using namespace hnrt;


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
