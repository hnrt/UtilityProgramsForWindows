#include "pch.h"
#include "hnrt/MultibyteString.h"
#include "hnrt/RefMbs.h"
#include "hnrt/Interlocked.h"
#include "hnrt/String.h"
#include "hnrt/Exception.h"


using namespace hnrt;


MultibyteString::MultibyteString()
    : m_psz(nullptr)
{
}


MultibyteString::MultibyteString(const MultibyteString& src)
    : m_psz(StringAddRef(src.m_psz))
{
}


MultibyteString::MultibyteString(PCSTR psz, SSIZE_T cb)
    : m_psz(nullptr)
{
    cb = StrLen(psz, cb);
    if (cb)
    {
        m_psz = RefMbs::Create(psz, cb);
    }
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
        m_psz = RefMbs::Create(cb);
        WideCharToMultiByte(cp, dwFlags, psz, static_cast<int>(cch), m_psz, cb, NULL, NULL);
        m_psz[cb] = '\0';
    }
}


MultibyteString::~MultibyteString()
{
    StringRelease(Interlocked<PSTR>::ExchangePointer(&m_psz, nullptr));
}


MultibyteString& MultibyteString::operator =(const MultibyteString& src)
{
    StringRelease(Interlocked<PSTR>::ExchangePointer(&m_psz, StringAddRef(src.m_psz)));
    return *this;
}


SIZE_T MultibyteString::get_Len() const
{
    return m_psz ? RefMbs::Get(m_psz).Len : 0;
}


MultibyteString hnrt::ToAcp(const String& str)
{
    return MultibyteString(CP_ACP, str.Ptr, str.Len);
}


MultibyteString hnrt::ToUTF8(const String& str)
{
    return MultibyteString(CP_UTF8, str.Ptr, str.Len);
}
