#pragma once


#include "hnrt/IMsiDatabase.h"


namespace hnrt
{
    struct MsiDatabaseFactory
    {
        static IMsiDatabase* OpenRead(PCWSTR pszPath);
    };
}
