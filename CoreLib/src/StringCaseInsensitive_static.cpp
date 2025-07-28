#include "pch.h"
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


int StringCaseInsensitive::Compare(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2)
{
    return StrCaseCmp(psz1, cch1, psz2, cch2);
}
