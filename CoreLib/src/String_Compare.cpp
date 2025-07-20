#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


int String::Compare(PCWSTR psz1, SSIZE_T cch1, PCWSTR psz2, SSIZE_T cch2)
{
    return StrCmp(psz1, cch1, psz2, cch2);
}
