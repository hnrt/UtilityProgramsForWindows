#pragma once


#include <Windows.h>
#include <Shlobj.h>


namespace hnrt
{
    struct Path
    {
        static const wchar_t DirectorySeparatorChar;
        static PCWSTR DirectorySeparator;

        static PCWSTR GetFileName(PCWSTR psz);
        static PCWSTR GetFileNameWithoutExtension(PCWSTR psz);
        static PCWSTR GetExtension(PCWSTR psz);
        static PCWSTR GetDirectoryName(PCWSTR psz, bool bEndsWithSeparator = false);
        static PCWSTR Combine(PCWSTR psz1, PCWSTR psz2, size_t cch2 = static_cast<size_t>(-1));
        static PCWSTR Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, size_t cch3 = static_cast<size_t>(-1));
        static PCWSTR Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, size_t cch4 = static_cast<size_t>(-1));
        static PCWSTR Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5, size_t cch5 = static_cast<size_t>(-1));
        static PCWSTR GetKnownFolder(const KNOWNFOLDERID& rid, DWORD dwFlags = 0);
        static int Compare(PCWSTR psz1, PCWSTR psz2);
        static int Compare(const char* psz1, const char* psz2);
        static bool WildcardMatch(PCWSTR pszPattern, PCWSTR pszSubject);
        static bool Exists(PCWSTR psz);
        static bool IsFile(PCWSTR psz);
        static bool IsDirectory(PCWSTR psz);
        static bool IsAbsolute(PCWSTR psz);
        static long long GetSize(PCWSTR psz);
        static bool ValidateDirectory(PCWSTR psz);
    };
}
