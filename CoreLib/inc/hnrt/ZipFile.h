#pragma once


#include "hnrt/ZipArchive.h"


namespace hnrt
{
    class ZipFile
    {
    public:

        static ZipArchive* OpenRead(PCWSTR pszPath);
        static void ExtractFiles(PCWSTR pszSource, PCWSTR pszFilePattern, PCWSTR pszDestination, IZipExtractCallbacks& callbacks);
    };
}
