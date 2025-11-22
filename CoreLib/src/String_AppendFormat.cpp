#include "pch.h"
#include <cstdarg>
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/Interlocked.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"


using namespace hnrt;


String& String::AppendFormat(PCWSTR pszFormat, ...)
{
    va_list argList;
    va_start(argList, pszFormat);
    SIZE_T cch1 = Len;
    SIZE_T cch2 = VaStrFmtLen(pszFormat, argList);
    PWSTR psz = RefStr::Create(cch1 + cch2);
    MemCpy(psz, m_psz, cch1);
    VaStrFmt(psz + cch1, cch2 + 1, pszFormat, argList);
    StringRelease(Interlocked<PWSTR>::ExchangePointer(&m_psz, psz));
    va_end(argList);
    return *this;
}
