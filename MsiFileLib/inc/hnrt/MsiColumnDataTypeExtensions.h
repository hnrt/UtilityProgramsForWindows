#pragma once


#include "hnrt/MsiColumnDataType.h"


namespace hnrt
{
    struct MsiColumnDataTypeExtensions
    {
        static MsiColumnDataType ToType(PCWSTR psz);
        static PCWSTR ToString(MsiColumnDataType type);
    };
}
