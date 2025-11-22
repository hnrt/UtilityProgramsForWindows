#include "pch.h"
#include <Windows.h>
#include "hnrt/String.h"
#include "hnrt/StringCommons.h"
#include "hnrt/RefString.h"
#include "hnrt/RefStr.h"
#include "hnrt/Exception.h"


using namespace hnrt;


String::String(UINT cp, PCSTR psz, SSIZE_T cb)
    : m_psz(nullptr)
{
    cb = StrLen(psz, cb);
    if (cb)
    {
        int cch = MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, static_cast<int>(cb), NULL, 0);
        if (cch <= 0)
        {
            throw Exception(L"String::ctor: MultiByteToWideChar failed.");
        }
        m_psz = RefStr::Create(cch);
        MultiByteToWideChar(cp, MB_PRECOMPOSED, psz, static_cast<int>(cb), m_psz, cch);
        m_psz[cch] = L'\0';
    }
    else
    {
        m_psz = StringAddRef(Empty.m_psz);
    }
}
