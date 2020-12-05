#pragma once


#include "hnrt/IMsiRow.h"


namespace hnrt
{
    struct IMsiRowCollection
    {
        virtual long get_Count() const = 0;
        virtual const IMsiRow& operator[](long rowNo) const = 0;
        virtual IMsiRow& operator[](long rowNo) = 0;
        virtual void Add(IMsiRow* pRow) = 0;

        __declspec(property(get = get_Count)) long Count;
    };
}
