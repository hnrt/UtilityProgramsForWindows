#pragma once


#include "hnrt/IMsiData.h"


namespace hnrt
{
    struct IMsiTable;

    struct IMsiRow
    {
        virtual ~IMsiRow() = default;
        virtual void SetData(long colNo, IMsiData* pData) = 0;
        virtual void SetData(PCWSTR colName, IMsiData* pData) = 0;
        virtual const IMsiData& operator[](long colNo) const = 0;
        virtual const IMsiData& operator[](PCWSTR colName) const = 0;
        virtual IMsiTable& get_Table() = 0;

        __declspec(property(get = get_Table)) IMsiTable& Table;
    };
}
