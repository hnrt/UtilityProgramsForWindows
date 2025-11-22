#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"


using namespace hnrt;


String& String::TruncateHead(SIZE_T cch)
{
    if (m_psz)
    {
        SIZE_T cch0 = Len;
        if (cch < cch0)
        {
            MemMove(m_psz, m_psz + cch, cch0 - cch);
            RefStr::Get(m_psz).Truncate(cch0 - cch);
        }
        else
        {
            RefStr::Get(m_psz).Truncate(0);
        }
    }
    return *this;
}
