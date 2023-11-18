#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/MsiColumnDataTypeExtensions.h"
#include "MsiColumn.h"
#include "MsiUtils.h"


using namespace hnrt;


MsiColumn::MsiColumn(PCWSTR pszColumn, bool bKey, PCWSTR pszNullable, PCWSTR pszMinValue, PCWSTR pszMaxValue, PCWSTR pszKeyTable, PCWSTR pszKeyColumn, PCWSTR pszCategory, PCWSTR pszSet, PCWSTR pszDescription)
    : IMsiColumn()
    , m_szName(pszColumn)
    , m_type(MsiColumnDataType::Text)
    , m_bKey(bKey)
    , m_bNullable(*pszNullable == L'Y')
    , m_szKeyTable(pszKeyTable)
    , m_szKeyColumn(pszKeyColumn)
    , m_szSet(pszSet)
    , m_szDescription(pszDescription)
{
    if (pszMinValue)
    {
        LONGLONG value = wcstoll(pszMinValue, nullptr, 0);
        if (value < -32767LL || value > +32767LL)
        {
            m_type = MsiColumnDataType::DoubleInteger;
        }
        else if (pszMaxValue)
        {
            value = wcstoll(pszMaxValue, nullptr, 0);
            if (value < -32767LL || value > +32767LL)
            {
                m_type = MsiColumnDataType::DoubleInteger;
            }
            else
            {
                m_type = MsiColumnDataType::Integer;
            }
        }
        else
        {
            m_type = MsiColumnDataType::Integer;
        }
    }
    else if (pszMaxValue)
    {
        LONGLONG value = wcstoll(pszMaxValue, nullptr, 0);
        if (value < -32767LL || value > +32767LL)
        {
            m_type = MsiColumnDataType::DoubleInteger;
        }
        else
        {
            m_type = MsiColumnDataType::Integer;
        }
    }
    else if (pszCategory)
    {
        m_type = MsiColumnDataTypeExtensions::ToType(pszCategory);
    }
}


MsiColumn::MsiColumn(const MsiColumn& src)
    : IMsiColumn()
    , m_szName(src.m_szName)
    , m_type(src.m_type)
    , m_bKey(src.m_bKey)
    , m_bNullable(src.m_bNullable)
    , m_szKeyTable(src.m_szKeyTable)
    , m_szKeyColumn(src.m_szKeyColumn)
    , m_szSet(src.m_szSet)
    , m_szDescription(src.m_szDescription)
{
}


PCWSTR MsiColumn::get_Name() const
{
    return m_szName;
}


MsiColumnDataType MsiColumn::get_Type() const
{
    return m_type;
}


bool MsiColumn::get_Key() const
{
    return m_bKey;
}


bool MsiColumn::get_Nullable() const
{
    return m_bNullable;
}


PCWSTR MsiColumn::get_KeyTable() const
{
    return m_szKeyTable;
}


PCWSTR MsiColumn::get_KeyColumn() const
{
    return m_szKeyColumn;
}


PCWSTR MsiColumn::get_Set() const
{
    return m_szSet;
}


PCWSTR MsiColumn::get_Description() const
{
    return m_szDescription;
}
