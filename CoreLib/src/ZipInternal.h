#pragma once


#include "hnrt/ZipArchive.h"


namespace hnrt
{
    class ZipInternal
    {
    public:

        static Folder* GetFolder(PCWSTR);
    };
}
