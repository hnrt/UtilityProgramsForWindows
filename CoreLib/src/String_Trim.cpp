#include "pch.h"
#include <Windows.h>
#include "hnrt/StringOptions.h"
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"


using namespace hnrt;


String String::Trim(StringOptions option) const
{
    if (Len)
    {
        int start = 0;
        int end = 0;
        StrTrimScan(m_psz, start, end, option);
        return String(m_psz + start, static_cast<SSIZE_T>(end - start));
    }
    else
    {
        return String();
    }
}
