#include "pch.h"
#include "MsiDataFactory.h"
#include "MsiUtils.h"
#include "hnrt/String.h"
#include "hnrt/Array.h"
#include "hnrt/StringUtils.h"
#include "hnrt/Heap.h"
#include "hnrt/Exception.h"


using namespace hnrt;


IMsiData* MsiDataFactory::CreateData(MSIHANDLE hRecord, UINT uField, MsiColumnDataType type)
{
    switch (type)
    {
    case MsiColumnDataType::Text:
    case MsiColumnDataType::UpperCase:
    case MsiColumnDataType::LowerCase:
    case MsiColumnDataType::Identifier:
    case MsiColumnDataType::Property:
    case MsiColumnDataType::Path:
    case MsiColumnDataType::RegPath:
    case MsiColumnDataType::Formatted:
    case MsiColumnDataType::FormattedSDDLText:
    case MsiColumnDataType::Template:
    case MsiColumnDataType::Condition:
    case MsiColumnDataType::GUID:
    case MsiColumnDataType::Language:
    case MsiColumnDataType::CustomSource:
    case MsiColumnDataType::Cabinet:
    case MsiColumnDataType::Shortcut:
        return new MsiText(hRecord, uField);

    case MsiColumnDataType::Integer:
        return new MsiInteger(hRecord, uField);

    case MsiColumnDataType::DoubleInteger:
        return new MsiDoubleInteger(hRecord, uField);

    case MsiColumnDataType::Time:
        return new MsiTime(hRecord, uField);

    case MsiColumnDataType::Date:
        return new MsiDate(hRecord, uField);

    case MsiColumnDataType::Filename:
    case MsiColumnDataType::WildCardFilename:
    case MsiColumnDataType::AnyPath:
    case MsiColumnDataType::DefaultDir:
        return new MsiFilename(hRecord, uField);

    case MsiColumnDataType::Paths:
        return new MsiPaths(hRecord, uField);

    case MsiColumnDataType::Version:
        return new MsiVersion(hRecord, uField);

    case MsiColumnDataType::Binary:
        return new MsiBinary(hRecord, uField);

    default:
        throw Exception(L"MsiDataFactory::CreateData: Bad type.");
    }
}


MsiText::MsiText(MSIHANDLE hRecord, UINT uField)
    : IMsiText()
    , m_bNull(MsiRecordIsNull(hRecord, uField) ? true : false)
    , m_szText(m_bNull ? nullptr : MsiUtils::GetString(hRecord, uField))
{
}


MsiInteger::MsiInteger(MSIHANDLE hRecord, UINT uField)
    : IMsiInteger()
    , m_bNull(MsiRecordIsNull(hRecord, uField) ? true : false)
    , m_szText(m_bNull ? nullptr : MsiUtils::GetString(hRecord, uField))
    , m_value(m_bNull ? MSI_NULL_INTEGER : wcstol(m_szText, nullptr, 10))
{
}


MsiDoubleInteger::MsiDoubleInteger(MSIHANDLE hRecord, UINT uField)
    : IMsiDoubleInteger()
    , m_bNull(MsiRecordIsNull(hRecord, uField) ? true : false)
    , m_szText(m_bNull ? nullptr : MsiUtils::GetString(hRecord, uField))
    , m_value(m_bNull ? MSI_NULL_INTEGER : wcstoll(m_szText, nullptr, 10))
{
}


MsiTime::MsiTime(MSIHANDLE hRecord, UINT uField)
    : IMsiTime()
    , m_bNull(MsiRecordIsNull(hRecord, uField) ? true : false)
    , m_szText(m_bNull ? nullptr : MsiUtils::GetString(hRecord, uField))
    , m_value(m_bNull ? MSI_NULL_INTEGER : wcstoul(m_szText, nullptr, 10))
{
}


MsiDate::MsiDate(MSIHANDLE hRecord, UINT uField)
    : IMsiDate()
    , m_bNull(MsiRecordIsNull(hRecord, uField) ? true : false)
    , m_szText(m_bNull ? nullptr : MsiUtils::GetString(hRecord, uField))
    , m_value(m_bNull ? MSI_NULL_INTEGER : wcstoul(m_szText, nullptr, 10))
{
}


MsiFilename::MsiFilename(MSIHANDLE hRecord, UINT uField)
    : IMsiFilename()
    , m_bNull(MsiRecordIsNull(hRecord, uField) ? true : false)
    , m_szText(m_bNull ? nullptr : MsiUtils::GetString(hRecord, uField))
    , m_szShortName(m_szText)
    , m_szLongName(m_szText)
{
    if (m_szText)
    {
        PCWSTR pSep = wcschr(m_szText, L'|');
        if (pSep)
        {
            m_szShortName = String(m_szText, pSep - m_szText.Ptr);
            m_szLongName = String(pSep + 1);
        }
    }
}


MsiPaths::MsiPaths(MSIHANDLE hRecord, UINT uField)
    : IMsiPaths()
    , m_bNull(MsiRecordIsNull(hRecord, uField) ? true : false)
    , m_szText(m_bNull ? nullptr : MsiUtils::GetString(hRecord, uField))
    , m_Paths()
{
    if (m_szText)
    {
        m_Paths = SplitBy(m_szText, L';');
    }
}


MsiVersion::MsiVersion(MSIHANDLE hRecord, UINT uField)
    : IMsiVersion()
    , m_bNull(MsiRecordIsNull(hRecord, uField) ? true : false)
    , m_szText(m_bNull ? nullptr : MsiUtils::GetString(hRecord, uField))
{
    if (m_szText)
    {
        PCWSTR pNext = m_szText;
        PWCHAR pStop;
        m_values[0] = wcstol(pNext, &pStop, 10);
        if (*pStop != L'.')
        {
            goto parse_error;
        }
        pNext = const_cast<PCWSTR>(pStop) + 1;
        m_values[1] = wcstol(pNext, &pStop, 10);
        if (*pStop != L'.')
        {
            goto parse_error;
        }
        pNext = const_cast<PCWSTR>(pStop) + 1;
        m_values[2] = wcstol(pNext, &pStop, 10);
        if (*pStop != L'.')
        {
            goto parse_error;
        }
        pNext = const_cast<PCWSTR>(pStop) + 1;
        m_values[3] = wcstol(pNext, &pStop, 10);
        if (*pStop != L'\0')
        {
            goto parse_error;
        }
    }
    else
    {
    parse_error:
        m_values[0] = 0;
        m_values[1] = 0;
        m_values[2] = 0;
        m_values[3] = 0;
    }
}


long MsiVersion::operator [](long index) const
{
    if (index < 0 || 4 <= index)
    {
        throw Exception(L"MsiVersion::operator []: Index out of range.");
    }
    return m_values[index];
}


MsiBinary::MsiBinary(MSIHANDLE hRecord, UINT uField)
    : IMsiBinary()
    , m_bNull(MsiRecordIsNull(hRecord, uField) ? true : false)
    , m_data()
{
    if (!m_bNull)
    {
        MsiUtils::ReadStream(hRecord, uField, m_data);
    }
}
