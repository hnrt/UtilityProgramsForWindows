#include "pch.h"
#include "String_internal.h"
#include "hnrt/Exception.h"


using namespace hnrt;


const String String::Empty = String();


String::String()
    : m_psz(nullptr)
{
}


String::String(const String& src)
    : m_psz(AddRef(src.m_psz))
{
}


String::String(PCWSTR psz, SSIZE_T cch)
    : m_psz(RefStr::Create(psz, cch))
{
}


String::String(PCWSTR pszFormat, va_list argList)
    : m_psz(RefStr::Create(pszFormat, argList))
{
}


String::String(StringOptions option, PCWSTR psz, ...)
    : m_psz(nullptr)
{
    va_list argList;
    va_start(argList, psz);
    switch (option)
    {
    case PRINTF:
        m_psz = RefStr::Create(psz, argList);
        break;
    case CONCAT:
    case CONCAT2:
    case CONCAT3:
    case CONCAT4:
    case CONCAT5:
    case CONCAT6:
    case CONCAT7:
    case CONCAT8:
    case CONCAT9:
        m_psz = RefStr::Create(option, psz, argList);
        break;
    case UPPERCASE:
    case LOWERCASE:
    case TRIM:
    case TRIM_HEAD:
    case TRIM_TAIL:
        m_psz = RefStr::Create(option, psz);
        break;
    default:
        throw Exception(L"String::ctor: Bad option.");
    }
    va_end(argList);
}


String::String(PCWSTR psz1, PCWSTR psz2)
    : m_psz(RefStr::Create(psz1, psz2))
{
}


String::String(SIZE_T cch)
    : m_psz(RefStr::Create(cch))
{
}


String::String(SIZE_T cch, WCHAR fill)
    : m_psz(RefStr::Create(cch, fill))
{
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
        m_psz = RefStr::Create(L"");
    }
}


SIZE_T String::get_len() const
{
    return m_psz ? RefStr::GetThis(m_psz)->Len : 0;
}


void String::set_len(SIZE_T len)
{
    if (m_psz)
    {
        RefStr::GetThis(m_psz)->SetLength(len);
    }
}
