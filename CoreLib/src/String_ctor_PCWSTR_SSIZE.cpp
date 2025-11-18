#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String::String(PCWSTR psz, SSIZE_T cch)
    : m_psz(nullptr)
{
    if (psz && cch)
    {
        cch = StrLen(psz, cch);
        m_psz = RefStr::Create(cch);
        MemCpy(m_psz, psz, cch);
        m_psz[cch] = L'\0';
    }
    else
    {
        m_psz = StringAddRef(Empty.m_psz);
    }
}
