#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String::String(SIZE_T cch, WCHAR fill)
    : m_psz(RefStr::Create(cch))
{
    if (cch)
    {
        MemSet(m_psz, fill, cch);
    }
    m_psz[cch] = L'\0';
}
