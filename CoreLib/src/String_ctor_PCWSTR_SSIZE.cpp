#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"


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
}
