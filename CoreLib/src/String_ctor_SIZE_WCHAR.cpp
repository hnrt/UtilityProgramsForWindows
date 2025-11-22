#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"


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
