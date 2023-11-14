#pragma once


#include "hnrt/String.h"


#define GUID_TOSTRING_BRACE     (0UL<<0)
#define GUID_TOSTRING_NO_BRACE  (1UL<<0)
#define GUID_TOSTRING_UPPERCASE (0UL<<1)
#define GUID_TOSTRING_LOWERCASE (1UL<<1)


namespace hnrt
{
    struct Guid
        : public ::GUID
    {
        Guid();
        Guid(const Guid& rhs) = default;
        ~Guid() = default;
        Guid& operator =(const Guid& rhs) = default;
        Guid& Initialize();
        String ToString(DWORD dwFlags = GUID_TOSTRING_BRACE | GUID_TOSTRING_UPPERCASE);
    };
}
