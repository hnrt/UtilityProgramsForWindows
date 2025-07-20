#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String String::Wrap(UINT width, PCWSTR pszNewLine) const
{
    if (!width)
    {
        return String(*this);
    }
    SIZE_T len = Len;
    SIZE_T lenNewLine = StrLen(pszNewLine);
    SIZE_T delta = (len / static_cast<SIZE_T>(width)) * lenNewLine;
    String sz(len + delta);
    PWCHAR pSrc = &m_psz[0];
    PWCHAR pEnd = &m_psz[len];
    PWCHAR pDst = sz.m_psz;
    while (pSrc + width <= pEnd)
    {
        pDst += MemCpy(pDst, pSrc, width);
        pDst += MemCpy(pDst, pszNewLine, lenNewLine);
        pSrc += width;
    }
    pDst += MemCpy(pDst, pSrc, pEnd - pSrc);
    pDst[0] = L'\0';
    return sz;
}
