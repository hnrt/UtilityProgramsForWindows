#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/MsiColumnDataTypeExtensions.h"
#include "MsiColumn.h"
#include "MsiUtils.h"


using namespace hnrt;


MsiColumn::MsiColumn(PCWSTR pszColumn, bool bKey, PCWSTR pszNullable, PCWSTR pszMinValue, PCWSTR pszMaxValue, PCWSTR pszKeyTable, PCWSTR pszKeyColumn, PCWSTR pszCategory, PCWSTR pszSet, PCWSTR pszDescription)
    : IMsiColumn()
    , m_pszName(pszColumn)
    , m_type(MsiColumnDataType::Text)
    , m_bKey(bKey)
    , m_bNullable(*pszNullable == L'Y')
    , m_pszKeyTable(pszKeyTable)
    , m_pszKeyColumn(pszKeyColumn)
    , m_pszSet(pszSet)
    , m_pszDescription(pszDescription)
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
    , m_pszName(src.m_pszName)
    , m_type(src.m_type)
    , m_bKey(src.m_bKey)
    , m_bNullable(src.m_bNullable)
    , m_pszKeyTable(src.m_pszKeyTable)
    , m_pszKeyColumn(src.m_pszKeyColumn)
    , m_pszSet(src.m_pszSet)
    , m_pszDescription(src.m_pszDescription)
{
}


PCWSTR MsiColumn::get_Name() const
{
    return m_pszName;
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
    return m_pszKeyTable;
}


PCWSTR MsiColumn::get_KeyColumn() const
{
    return m_pszKeyColumn;
}


PCWSTR MsiColumn::get_Set() const
{
    return m_pszSet;
}


PCWSTR MsiColumn::get_Description() const
{
    return m_pszDescription;
}
