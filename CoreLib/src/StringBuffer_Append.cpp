#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/StringCommons.h"
#include "hnrt/String.h"


using namespace hnrt;


StringBuffer& StringBuffer::Append(const StringBuffer& other)
{
    if (other.Len)
    {
        CheckCapacity(other.Len);
        m_len += StrCpy(m_ptr + m_len, other.Ptr, other.Len);
    }
    return *this;
}


StringBuffer& StringBuffer::Append(const String& other)
{
    if (other.Len)
    {
        CheckCapacity(other.Len);
        m_len += StrCpy(m_ptr + m_len, other.Ptr, other.Len);
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
                m_len += StrCpy(m_ptr + m_len, psz, cch);
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
            m_len += StrCpy(m_ptr + m_len, psz, cch);
        }
    }
    return *this;
}


StringBuffer& StringBuffer::Append(int c)
{
    if (IS_BMP(c))
    {
        CheckCapacity(1);
        m_ptr[m_len++] = c;
        m_ptr[m_len] = L'\0';
    }
    else
    {
        CheckCapacity(2);
        m_ptr[m_len++] = HIGH_SURROGATE(c);
        m_ptr[m_len++] = LOW_SURROGATE(c);
        m_ptr[m_len] = L'\0';
    }
    return *this;
}
