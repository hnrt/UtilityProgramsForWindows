#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/NumberText.h"
#include "NumberTextInternal.h"


using namespace hnrt;


NumberFormatter::NumberFormatter()
{
    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SGROUPING, m_szGrouping, _countof(m_szGrouping)))
    {
        wcscpy_s(m_szGrouping, L"3;0");
    }
    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL, m_szDecimal, _countof(m_szDecimal)))
    {
        wcscpy_s(m_szDecimal, L".");
    }
    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_STHOUSAND, m_szThousand, _countof(m_szThousand)))
    {
        wcscpy_s(m_szThousand, L",");
    }
    UINT uNegNumber = 0UL;
    if (!GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_INEGNUMBER | LOCALE_RETURN_NUMBER, reinterpret_cast<LPWSTR>(&uNegNumber), sizeof(uNegNumber)))
    {
        uNegNumber = 1UL;
    }
    m_numfmt.NumDigits = 0;
    m_numfmt.LeadingZero = 0;
    if (L'0' <= m_szGrouping[0] && m_szGrouping[0] <= L'9')
    {
        m_numfmt.Grouping = m_szGrouping[0] - L'0';
        if (m_szGrouping[1] == L';')
        {
            if (L'1' <= m_szGrouping[2] && m_szGrouping[2] <= L'9')
            {
                m_numfmt.Grouping = m_numfmt.Grouping * 10 + m_szGrouping[2] - L'0';
            }
        }
    }
    else
    {
        m_numfmt.Grouping = 3;
    }
    m_numfmt.lpDecimalSep = m_szDecimal;
    m_numfmt.lpThousandSep = m_szThousand;
    m_numfmt.NegativeOrder = uNegNumber;
}


static NumberFormatter s_NumberFormatter;


NumberText::NumberText(int value)
    : StringBuffer(32)
{
    WCHAR sz[32];
    _snwprintf_s(sz, _TRUNCATE, L"%d", value);
    GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, sz, &s_NumberFormatter, m_ptr, static_cast<int>(m_cap));
}


NumberText::NumberText(unsigned int value)
    : StringBuffer(32)
{
    WCHAR sz[32];
    _snwprintf_s(sz, _TRUNCATE, L"%u", value);
    GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, sz, &s_NumberFormatter, m_ptr, static_cast<int>(m_cap));
}


NumberText::NumberText(short value)
    : StringBuffer(16)
{
    WCHAR sz[16];
    _snwprintf_s(sz, _TRUNCATE, L"%d", value);
    GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, sz, &s_NumberFormatter, m_ptr, static_cast<int>(m_cap));
}


NumberText::NumberText(unsigned short value)
    : StringBuffer(16)
{
    WCHAR sz[16];
    _snwprintf_s(sz, _TRUNCATE, L"%u", value);
    GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, sz, &s_NumberFormatter, m_ptr, static_cast<int>(m_cap));
}


NumberText::NumberText(long value)
    : StringBuffer(32)
{
    WCHAR sz[32];
    _snwprintf_s(sz, _TRUNCATE, L"%ld", value);
    GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, sz, &s_NumberFormatter, m_ptr, static_cast<int>(m_cap));
}


NumberText::NumberText(unsigned long value)
    : StringBuffer(32)
{
    WCHAR sz[32];
    _snwprintf_s(sz, _TRUNCATE, L"%lu", value);
    GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, sz, &s_NumberFormatter, m_ptr, static_cast<int>(m_cap));
}


NumberText::NumberText(long long value)
    : StringBuffer(64)
{
    WCHAR sz[64];
    _snwprintf_s(sz, _TRUNCATE, L"%lld", value);
    GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, sz, &s_NumberFormatter, m_ptr, static_cast<int>(m_cap));
}


NumberText::NumberText(unsigned long long value)
    : StringBuffer(64)
{
    WCHAR sz[64];
    _snwprintf_s(sz, _TRUNCATE, L"%llu", value);
    GetNumberFormatEx(LOCALE_NAME_USER_DEFAULT, 0, sz, &s_NumberFormatter, m_ptr, static_cast<int>(m_cap));
}
