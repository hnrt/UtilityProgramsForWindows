#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String String::Format(PCWSTR pszFormat, ...)
{
    String str;
    va_list argList;
    va_start(argList, pszFormat);
    SIZE_T cch = VaStrFmtLen(pszFormat, argList);
    str.m_psz = RefStr::Create(cch);
    VaStrFmt(str.m_psz, cch + 1, pszFormat, argList);
    va_end(argList);
    return str;
}
