#include "pch.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


StringBuffer& StringBuffer::Format(PCWSTR pszFormat, ...)
{
    if (m_ptr)
    {
        m_len = 0;
        m_ptr[0] = L'\0';
    }
    va_list argList;
    va_start(argList, pszFormat);
    VaAppendFormat(pszFormat, argList);
    va_end(argList);
    return *this;
}
