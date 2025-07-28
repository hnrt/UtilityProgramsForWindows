#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::Trim(StringOptions option)
{
    if (Len)
    {
        int start = 0;
        int end = 0;
        StrTrimScan(m_psz, start, end, option);
        SIZE_T cch = static_cast<SIZE_T>(end - start);
        if (cch < Len)
        {
            MemMove(m_psz, m_psz + start, cch);
            RefStr::Get(m_psz).Truncate(cch);
        }
    }
    return *this;
}
