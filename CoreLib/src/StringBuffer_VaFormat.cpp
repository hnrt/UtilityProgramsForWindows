#include "pch.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


StringBuffer& StringBuffer::VaFormat(PCWSTR pszFormat, va_list argList)
{
    if (m_ptr)
    {
        m_len = 0;
        m_ptr[0] = L'\0';
    }
    return VaAppendFormat(pszFormat, argList);
}
