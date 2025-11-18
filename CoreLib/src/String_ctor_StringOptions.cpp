#include "pch.h"
#include "String_internal.h"
#include "hnrt/Exception.h"


using namespace hnrt;


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
