#include "pch.h"
#include "String_internal.h"
#include "hnrt/Exception.h"


using namespace hnrt;


const String String::Empty = String(0ULL, L'\0');


String::String()
    : m_psz(StringAddRef(Empty.m_psz))
{
}


String::String(const String& src)
    : m_psz(StringAddRef(src.m_psz))
{
}


String::String(PCWSTR psz, SSIZE_T cch)
    : m_psz(nullptr)
{
    if (psz && cch)
    {
        cch = StrLen(psz, cch);
        m_psz = RefStr::Create(cch);
        MemCpy(m_psz, psz, cch);
        m_psz[cch] = L'\0';
    }
    else
    {
        m_psz = StringAddRef(Empty.m_psz);
    }
}


String::String(PCWSTR pszFormat, va_list argList)
    : m_psz(RefStr::Create(VaStrFmtLen(pszFormat, argList)))
{
    VaStrFmt(m_psz, Len + 1, pszFormat, argList);
}


String::String(StringOptions option, PCWSTR psz, ...)
    : m_psz(nullptr)
{
    va_list argList;
    va_start(argList, psz);
    switch (option)
    {
    case CONCAT:
    case CONCAT2:
    case CONCAT3:
    case CONCAT4:
    case CONCAT5:
    case CONCAT6:
    case CONCAT7:
    case CONCAT8:
    case CONCAT9:
        m_psz = RefStr::Create(VaStrCatLen(option, psz, argList));
        VaStrCat(option, psz, argList, m_psz);
        break;
    case UPPERCASE:
    case LOWERCASE:
        m_psz = RefStr::Create(StrLen(psz));
        MemCpy(m_psz, psz, Len);
        m_psz[Len] = L'\0';
        StrCase(option, m_psz, Len);
        break;
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
    {
        int start = 0;
        int end = 0;
        StrTrimScan(psz, start, end, option);
        m_psz = RefStr::Create(static_cast<SIZE_T>(end - start));
        MemCpy(m_psz, psz + start, Len);
        m_psz[Len] = L'\0';
        break;
    }
    default:
        throw Exception(L"String::ctor: Bad option.");
    }
    va_end(argList);
}


String::String(PCWSTR psz1, PCWSTR psz2)
    : m_psz(nullptr)
{
    SIZE_T cch1 = StrLen(psz1);
    SIZE_T cch2 = StrLen(psz2);
    m_psz = RefStr::Create(cch1 + cch2);
    MemCpy(m_psz, psz1, cch1);
    MemCpy(m_psz + cch1, psz2, cch2 + 1);
}


String::String(SIZE_T cch, WCHAR fill)
    : m_psz(RefStr::Create(cch))
{
    if (cch)
    {
        MemSet(m_psz, fill, cch);
    }
    m_psz[cch] = L'\0';
}


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


SIZE_T String::get_Len() const
{
    return m_psz ? RefStr::Get(m_psz).Len : 0;
}
