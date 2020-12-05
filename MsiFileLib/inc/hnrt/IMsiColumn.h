#pragma once


#include <Windows.h>
#include "hnrt/MsiColumnDataType.h"


namespace hnrt
{
    struct IMsiColumn
    {
        virtual PCWSTR get_Name() const = 0;
        virtual MsiColumnDataType get_Type() const = 0;
        virtual bool get_Key() const = 0;
        virtual bool get_Nullable() const = 0;
        virtual PCWSTR get_KeyTable() const = 0;
        virtual PCWSTR get_KeyColumn() const = 0;
        virtual PCWSTR get_Set() const = 0;
        virtual PCWSTR get_Description() const = 0;

        __declspec(property(get = get_Name)) PCWSTR Name;
        __declspec(property(get = get_Type)) MsiColumnDataType Type;
        __declspec(property(get = get_Key)) bool Key;
        __declspec(property(get = get_Nullable)) bool Nullable;
        __declspec(property(get = get_KeyTable)) PCWSTR KeyTable;
        __declspec(property(get = get_KeyColumn)) PCWSTR KeyColumn;
        __declspec(property(get = get_Set)) PCWSTR Set;
        __declspec(property(get = get_Description)) PCWSTR Description;
    };
}
