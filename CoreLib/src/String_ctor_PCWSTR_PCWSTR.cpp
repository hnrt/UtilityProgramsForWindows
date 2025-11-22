#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefStr.h"


using namespace hnrt;


String::String(PCWSTR psz1, PCWSTR psz2)
    : m_psz(nullptr)
{
    SIZE_T cch1 = StrLen(psz1);
    SIZE_T cch2 = StrLen(psz2);
    m_psz = RefStr::Create(cch1 + cch2);
    MemCpy(m_psz, psz1, cch1);
    MemCpy(m_psz + cch1, psz2, cch2 + 1);
}
