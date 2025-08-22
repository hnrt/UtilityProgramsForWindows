#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


int String::CompareCaseInsensitively(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2)
{
    return StrCaseCmp(psz1, cch1, psz2, cch2);
}
