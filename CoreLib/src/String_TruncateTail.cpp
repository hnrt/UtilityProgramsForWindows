#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::TruncateTail(SIZE_T cch)
{
    if (m_psz)
    {
        SIZE_T cch0 = Len;
        if (cch < cch0)
        {
            RefStr::Get(m_psz).Truncate(cch0 - cch);
        }
        else
        {
            RefStr::Get(m_psz).Truncate(0);
        }
    }
    return *this;
}
