#pragma once


#include "hnrt/String.h"
#include <Shlobj.h>


namespace hnrt
{
    struct Path
    {
        static const WCHAR DirectorySeparatorChar;
        static PCWSTR DirectorySeparator;

        static String GetFileName(PCWSTR psz);
        static String GetFileNameWithoutExtension(PCWSTR psz);
        static String GetExtension(PCWSTR psz);
        static String GetDirectoryName(PCWSTR psz, bool bEndsWithSeparator = false);
        static String Combine(PCWSTR psz1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static String Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, size_t cch3 = static_cast<size_t>(-1));
        static String Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, size_t cch4 = static_cast<size_t>(-1));
        static String Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5, size_t cch5 = static_cast<size_t>(-1));
        static String GetKnownFolder(const KNOWNFOLDERID& rid, DWORD dwFlags = 0);
        static int Compare(PCWSTR psz1, PCWSTR psz2);
        static int Compare(PCSTR psz1, PCSTR psz2);
        static bool WildcardMatch(PCWSTR pszPattern, PCWSTR pszSubject);
        static bool Exists(PCWSTR psz);
        static bool IsFile(PCWSTR psz);
        static bool IsDirectory(PCWSTR psz);
        static bool IsAbsolute(PCWSTR psz);
        static INT64 GetSize(PCWSTR psz);
        static bool ValidateDirectory(PCWSTR psz);
    };
}
