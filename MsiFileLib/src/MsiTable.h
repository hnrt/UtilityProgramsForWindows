#pragma once


#include <map>
#include "hnrt/String.h"
#include "hnrt/IMsiTable.h"
#include "MsiDatabase.h"
#include "MsiColumnCollection.h"
#include "MsiRowCollection.h"


namespace hnrt
{
    class MsiTable
        : public IMsiTable
    {
    public:

        MsiTable(PCWSTR pszName, MsiDatabase& database);
        MsiTable(const MsiTable&) = delete;
        virtual ~MsiTable();
        virtual long AddRef();
        virtual long Release();
        virtual IMsiRow* NewRow();
        virtual PCWSTR get_Name() const;
        virtual const IMsiColumnCollection& get_Columns() const;
        virtual const IMsiRowCollection& get_Rows() const;
        virtual long Find(PCWSTR pszColumnName, PCWSTR pszValue, long startRowNo) const;
        virtual void CreateIndex();

    private:

        void Load();

        typedef std::map<PCWSTR, long, StringLessThan> Index;
        typedef std::pair<PCWSTR, long> IndexEntry;

        long m_refs;
        PCWSTR m_pszName;
        MsiDatabase& m_database;
        MsiColumnCollection m_columns;
        MsiRowCollection m_rows;
        Index* m_pIndex;
    };
}
