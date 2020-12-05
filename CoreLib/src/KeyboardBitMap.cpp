#include "pch.h"
#include "hnrt/KeyboardBitMap.h"
#include "hnrt/Debug.h"
#include "hnrt/Buffer.h"
#include "hnrt/VirtualKey.h"


#define DELIMITER L"+"


using namespace hnrt;


static PWSTR Trim(PWSTR psz)
{
    PWCHAR pR = psz;
    while (iswspace(*pR))
    {
        pR++;
    }
    PWCHAR pStart = pR;
    PWCHAR pEnd = nullptr;
    while (*pR)
    {
        if (!iswspace(*pR))
        {
            pEnd = nullptr;
        }
        else if (!pEnd)
        {
            pEnd = pR;
        }
        pR++;
    }
    if (pEnd)
    {
        *pEnd = L'\0';
    }
    return pStart;
}


int KeyboardBitMap::Parse(PCWSTR psz, int* piErrors)
{
    DBGFNC(L"KeyboardBitMap::Parse");
    DBGPUT(psz ? L"input=\"%s\"" : L"input=NULL", psz);
    if (!psz || !*psz)
    {
        if (piErrors)
        {
            *piErrors = 0;
        }
        DBGPUT(L"return=0 errors=0");
        return 0;
    }
    Buffer<WCHAR> szCopy(wcslen(psz) + 1 + 3);
    wcscpy_s(szCopy + 3, szCopy.Len - 3, psz);
    int iParsed = 0;
    int iErrors = 0;
    PWSTR context = NULL;
    PWSTR pszNext = wcstok_s(szCopy + 3, DELIMITER, &context);
    while (pszNext)
    {
        pszNext = Trim(pszNext);
        pszNext[-3] = L'V';
        pszNext[-2] = L'K';
        pszNext[-1] = L'_';
        VirtualKey key(pszNext - 3);
        DBGPUT(L"next=\"%s\" code=%d", pszNext, key.Value);
        if (0 < key.Value && key.Value < 255)
        {
            Set(key.Value);
            iParsed++;
        }
        else
        {
            PWSTR pStop = nullptr;
            DWORD code = static_cast<DWORD>(wcstoul(pszNext, &pStop, 0));
            if (pszNext < pStop && !*pStop && 0 < code && code < 255)
            {
                Set(code);
                iParsed++;
            }
            else
            {
                iErrors++;
            }
        }
        pszNext = wcstok_s(NULL, DELIMITER, &context);
    }
    if (piErrors)
    {
        *piErrors = iErrors;
    }
    DBGPUT(L"return=%d errors=%d", iParsed, iErrors);
    return iParsed;
}
