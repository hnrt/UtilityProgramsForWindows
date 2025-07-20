#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String String::Replace(PCWSTR psz1, PCWSTR psz2, int count) const
{
    if (m_psz)
    {
        SSIZE_T len = Len;
        SSIZE_T len1 = StrLen(psz1);
        PWCHAR pEnd = m_psz + len;
        PWCHAR pCur = m_psz;
        int cnt1 = 0;
        while (pCur < pEnd)
        {
            SSIZE_T index = hnrt::IndexOf(pCur, psz1, pEnd - pCur);
            if (index < 0 || ++cnt1 >= count)
            {
                break;
            }
            pCur += index + len1;
        }
        if (cnt1)
        {
            SSIZE_T len2 = StrLen(psz2);
            SSIZE_T delta = (len2 - len1) * cnt1;
            if (len + delta)
            {
                String sz(len + delta);
                PWCHAR pDst = sz.m_psz;
                pCur = m_psz;
                while (cnt1--)
                {
                    SSIZE_T index = hnrt::IndexOf(pCur, psz1, pEnd - pCur);
                    pDst += MemCpy(pDst, pCur, index);
                    pDst += MemCpy(pDst, psz2, len2);
                    pCur += index + len1;
                }
                pDst += MemCpy(pDst, pCur, pEnd - pCur);
                pDst[0] = static_cast<WCHAR>(0);
                return sz;
            }
        }
    }
    return *this;
}
