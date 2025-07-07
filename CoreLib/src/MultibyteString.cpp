#include "pch.h"
#include "hnrt/MultibyteString.h"
#include "hnrt/RefString.h"
#include "hnrt/Interlocked.h"
#include "hnrt/Exception.h"


using namespace hnrt;


typedef RefString<CHAR> RefStr;


inline static PSTR AddRef(PSTR psz)
{
    if (psz)
    {
        RefStr::GetThis(psz)->AddRef();
    }
    return psz;
}


inline static void Release(PSTR psz)
{
    if (psz)
    {
        RefStr::GetThis(psz)->Release();
    }
}


MultibyteString::MultibyteString()
    : m_psz(nullptr)
{
}


MultibyteString::MultibyteString(const MultibyteString& src)
    : m_psz(AddRef(src.m_psz))
{
}


MultibyteString::MultibyteString(PCSTR psz, SSIZE_T cb)
    : m_psz(RefStr::Create(psz, cb))
{
}


MultibyteString::MultibyteString(UINT cp, PCWSTR psz, SSIZE_T cch)
    : m_psz(nullptr)
{
    cch = StrLen(psz, cch);
    if (cch)
    {
        DWORD dwFlags = (cp == CP_UTF8 || cp == 54936) ? WC_ERR_INVALID_CHARS : 0;
        int cb = WideCharToMultiByte(cp, dwFlags, psz, static_cast<int>(cch), NULL, 0, NULL, NULL);
        if (cb <= 0)
        {
            throw Exception(L"MultibyteString::ctor: WideCharToMultiByte failed.");
        }
        m_psz = RefStr::Create(cb);
        WideCharToMultiByte(cp, dwFlags, psz, static_cast<int>(cch), m_psz, cb, NULL, NULL);
        m_psz[cb] = '\0';
    }
    else
    {
        m_psz = RefStr::Create("");
    }
}


MultibyteString::~MultibyteString()
{
    Release(Interlocked<PSTR>::ExchangePointer(&m_psz, nullptr));
}


MultibyteString& MultibyteString::operator =(const MultibyteString& src)
{
    Release(Interlocked<PSTR>::ExchangePointer(&m_psz, AddRef(src.m_psz)));
    return *this;
}


SIZE_T MultibyteString::get_len() const
{
    return m_psz ? RefStr::GetThis(m_psz)->Len : 0;
}
