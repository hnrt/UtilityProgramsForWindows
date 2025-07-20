#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String String::ChangeLineBreak(LineBreak lbSpec) const
{
    SIZE_T len = Len;
    if (len > 0 && (lbSpec == LineBreak::CRLF || lbSpec == LineBreak::LF))
    {
        SIZE_T cLF = 0;
        SIZE_T cCRLF = 0;
        PWCHAR pCur = &m_psz[0];
        PWCHAR pStop = &m_psz[len - 1];
        PWCHAR pEnd = &m_psz[len];
        while (pCur < pStop)
        {
            WCHAR c = *pCur++;
            if (c == L'\n')
            {
                cLF++;
            }
            else if (c == L'\r' && *pCur == L'\n')
            {
                cCRLF++;
                pCur++;
            }
        }
        if (pCur < pEnd && *pCur == L'\n')
        {
            cLF++;
        }
        if (lbSpec == LineBreak::CRLF)
        {
            if (cLF > 0)
            {
                String sz(len + cLF);
                PWCHAR pDst = sz.m_psz;
                pCur = &m_psz[0];
                while (pCur < pStop)
                {
                    WCHAR c = *pCur++;
                    if (c == L'\n')
                    {
                        *pDst++ = L'\r';
                        *pDst++ = L'\n';
                    }
                    else if (c == L'\r' && *pCur == L'\n')
                    {
                        *pDst++ = L'\r';
                        *pDst++ = L'\n';
                        pCur++;
                    }
                    else
                    {
                        *pDst++ = c;
                    }
                }
                if (pCur < pEnd)
                {
                    WCHAR c = *pCur++;
                    if (c == L'\n')
                    {
                        *pDst++ = L'\r';
                        *pDst++ = L'\n';
                    }
                    else
                    {
                        *pDst++ = c;
                    }
                }
                *pDst = L'\0';
                return sz;
            }
        }
        else if (cCRLF > 0)
        {
            String sz(len - cCRLF);
            PWCHAR pDst = sz.m_psz;
            pCur = &m_psz[0];
            while (pCur < pStop)
            {
                WCHAR c = *pCur++;
                if (c == L'\r' && *pCur == L'\n')
                {
                    *pDst++ = L'\n';
                    pCur++;
                }
                else
                {
                    *pDst++ = c;
                }
            }
            if (pCur < pEnd)
            {
                *pDst++ = *pCur++;
            }
            *pDst = L'\0';
            return sz;
        }
    }
    return String(*this);
}
