#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


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
