#include "pch.h"
#include "String_internal.h"


using namespace hnrt;


String& String::TruncateTail(SIZE_T cch)
{
    if (m_psz)
    {
        RefStr* pThis = RefStr::GetThis(m_psz);
        if (pThis->RefCnt > 1)
        {
            Release(Interlocked<PWSTR>::ExchangePointer(&m_psz, cch < pThis->Len ? RefStr::Create(m_psz, pThis->Len - cch) : nullptr));
        }
        else if (cch < pThis->Len)
        {
            SIZE_T newLen = pThis->Len - cch;
            MemSet(&m_psz[newLen], L'\0', cch);
            pThis->SetLength(newLen);
        }
        else
        {
            pThis->Fill(L'\0');
        }
    }
    return *this;
}
