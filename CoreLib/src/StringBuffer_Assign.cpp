#include "pch.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


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


StringBuffer& StringBuffer::Assign(int c)
{
    if (m_ptr)
    {
        m_len = 0;
        m_ptr[0] = L'\0';
    }
    return Append(c);
}
