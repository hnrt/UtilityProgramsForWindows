#pragma once


#include "hnrt/IMsiColumn.h"


namespace hnrt
{
    struct IMsiColumnCollection
    {
        virtual long get_Count() const = 0;
        virtual PCWSTR get_PrimaryKey() const = 0;
        virtual long GetKeyIndex(long keyNo) const = 0;
        virtual long get_KeyCount() const = 0;
        virtual const IMsiColumn& operator[](long colNo) const = 0;
        virtual IMsiColumn& operator[](long colNo) = 0;
        virtual const IMsiColumn& operator[](PCWSTR colName) const = 0;
        virtual IMsiColumn& operator[](PCWSTR colName) = 0;
        virtual long IndexOf(PCWSTR colName) const = 0;

        __declspec(property(get = get_Count)) long Count;
        __declspec(property(get = get_PrimaryKey)) PCWSTR PrimaryKey;
        __declspec(property(get = get_KeyCount)) long KeyCount;
    };
}
