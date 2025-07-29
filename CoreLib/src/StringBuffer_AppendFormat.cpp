#include "pch.h"
#include "hnrt/StringBuffer.h"


using namespace hnrt;


StringBuffer& StringBuffer::AppendFormat(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    VaAppendFormat(pszFormat, argList);
    va_end(argList);
    return *this;
}
