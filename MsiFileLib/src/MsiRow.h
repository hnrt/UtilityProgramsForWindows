#pragma once


#include "hnrt/IMsiRow.h"


namespace hnrt
{
    class MsiRow
        : public IMsiRow
    {
    public:

        virtual ~MsiRow();
        MsiRow(IMsiTable* pTable);
        MsiRow(const MsiRow&) = delete;
        virtual void SetData(long colNo, IMsiData* pData);
        virtual void SetData(PCWSTR colName, IMsiData* pData);
        virtual const IMsiData& operator[](long colNo) const;
        virtual const IMsiData& operator[](PCWSTR colName) const;
        virtual IMsiTable& get_Table();

    private:

        IMsiTable* m_pTable;
        IMsiData** m_ppItems;
    };
}
