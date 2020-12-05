#pragma once


#include "hnrt/IMsiColumnCollection.h"
#include "hnrt/IMsiRowCollection.h"


namespace hnrt
{
    struct IMsiTable
    {
        virtual long AddRef() = 0;
        virtual long Release() = 0;
        virtual IMsiRow* NewRow() = 0;
        virtual PCWSTR get_Name() const = 0;
        virtual const IMsiColumnCollection& get_Columns() const = 0;
        virtual const IMsiRowCollection& get_Rows() const = 0;
        virtual long Find(PCWSTR pszColumnName, PCWSTR pszValue, long startRowNo = 0) const = 0;
        virtual void CreateIndex() = 0;

        __declspec(property(get = get_Name)) PCWSTR Name;
        __declspec(property(get = get_Columns)) const IMsiColumnCollection& Columns;
        __declspec(property(get = get_Rows)) const IMsiRowCollection& Rows;
    };
}
