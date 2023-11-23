#pragma once


#include "hnrt/DirectoryEntry.h"
#include <Shlobj.h>
#include <vector>


namespace hnrt
{
    struct Path
    {
        static const WCHAR DirectorySeparatorChar;
        static const WCHAR DirectorySeparator[2];

        static String GetFileName(PCWSTR psz);
        static String GetFileNameWithoutExtension(PCWSTR psz);
        static String GetExtension(PCWSTR psz);
        static String GetDirectoryName(PCWSTR psz, bool bEndsWithSeparator = false);
        static String Combine(PCWSTR psz1, PCWSTR psz2, INT_PTR cch2 = -1);
        static String Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, INT_PTR cch3 = -1);
        static String Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, INT_PTR cch4 = -1);
        static String Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5, INT_PTR cch5 = -1);
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
        static std::vector<DirectoryEntry>& ListFiles(std::vector<DirectoryEntry>& entries, PCWSTR psz, PCWSTR pszPattern = L"*");
        static std::vector<DirectoryEntry>& ListDirectories(std::vector<DirectoryEntry>& entries, PCWSTR psz, PCWSTR pszPattern = L"*");
    };
}
