#include "pch.h"
#include "hnrt/Guid.h"
#include "hnrt/ComException.h"
#include <combaseapi.h>


using namespace hnrt;


Guid::Guid()
{
    memset(this, 0, sizeof(*this));
}


Guid& Guid::Initialize()
{
    HRESULT hr = CoCreateGuid(this);
    if (hr != S_OK)
    {
        throw ComException(hr, L"CoCreateGuid failed.");
    }
    return *this;
}


String Guid::ToString(DWORD dwFlags)
{
    OLECHAR buf[256] = { 0 };
    if (!StringFromGUID2(*this, buf, 256))
    {
        throw Exception(L"StringFromGUID2 failed.");
    }
    PWCHAR pStart = buf;
    PWCHAR pEnd = buf + wcslen(buf);
    if ((dwFlags & GUID_TOSTRING_NO_BRACE))
    {
        for (PWCHAR pCur = pStart; pCur < pEnd; pCur++)
        {
            if (*pCur == L'{')
            {
                pStart = pCur + 1;
                for (pCur = pEnd - 1; pStart <= pCur; pCur--)
                {
                    if (*pCur == L'}')
                    {
                        pEnd = pCur;
                        break;
                    }
                }
                break;
            }
        }
    }
    if ((dwFlags & GUID_TOSTRING_LOWERCASE))
    {
        for (PWCHAR pCur = pStart; pCur < pEnd; pCur++)
        {
            *pCur = towlower(*pCur);
        }
    }
    return String(pStart, pEnd - pStart);
}
