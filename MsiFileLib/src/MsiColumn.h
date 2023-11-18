#pragma once


#include "hnrt/String.h"
#include "hnrt/IMsiColumn.h"


namespace hnrt
{
    class MsiColumn
        : public IMsiColumn
    {
    public:

        MsiColumn(PCWSTR pszColumn, bool bKey, PCWSTR pszNullable, PCWSTR pszMinValue, PCWSTR pszMaxValue, PCWSTR pszKeyTable, PCWSTR pszKeyColumn, PCWSTR pszCategory, PCWSTR pszSet, PCWSTR pszDescription);
        MsiColumn(const MsiColumn&);
        virtual ~MsiColumn() = default;
        void operator =(const MsiColumn&) = delete;
        virtual PCWSTR get_Name() const;
        virtual MsiColumnDataType get_Type() const;
        virtual bool get_Key() const;
        virtual bool get_Nullable() const;
        virtual PCWSTR get_KeyTable() const;
        virtual PCWSTR get_KeyColumn() const;
        virtual PCWSTR get_Set() const;
        virtual PCWSTR get_Description() const;

    private:

        String m_szName;
        MsiColumnDataType m_type;
        bool m_bKey;
        bool m_bNullable;
        String m_szKeyTable;
        String m_szKeyColumn;
        String m_szSet;
        String m_szDescription;
    };
}
