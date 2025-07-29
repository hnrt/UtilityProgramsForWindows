#include "pch.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


StringBuffer& StringBuffer::VaAppendFormat(PCWSTR pszFormat, va_list argList)
{
    CheckCapacity(VaStrFmtLen(pszFormat, argList));
    m_len += VaStrFmt(m_ptr + m_len, m_cap - m_len, pszFormat, argList);
    return *this;
}
