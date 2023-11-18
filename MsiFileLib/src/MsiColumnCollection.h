#pragma once


#include <Msi.h>
#include "hnrt/String.h"
#include "hnrt/IMsiColumnCollection.h"
#include "MsiColumn.h"


namespace hnrt
{
    class MsiColumnCollection
        : public IMsiColumnCollection
    {
    public:

        MsiColumnCollection();
        MsiColumnCollection(MSIHANDLE hDatabase, PCWSTR pszTableName);
        MsiColumnCollection(const MsiColumnCollection&) = delete;
        virtual ~MsiColumnCollection();
        void operator =(const MsiColumnCollection&) = delete;
        virtual long get_Count() const;
        virtual PCWSTR get_PrimaryKey() const;
        virtual long GetKeyIndex(long keyNo) const;
        virtual long get_KeyCount() const;
        virtual const IMsiColumn& operator[](long colNo) const;
        virtual IMsiColumn& operator[](long colNo);
        virtual const IMsiColumn& operator[](PCWSTR colName) const;
        virtual IMsiColumn& operator[](PCWSTR colName);
        virtual long IndexOf(PCWSTR colName) const;

    private:

        long m_count;
        MsiColumn* m_columns;
        String m_szPrimaryKey;
        long m_keyList[32];
        long m_keyCount;
        long m_next;
    };
}
