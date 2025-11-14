#pragma once

#include "hnrt/DirectoryEntry.h"
#include "hnrt/Array.h"
#include <Shlobj.h>

#ifdef GetCurrentDirectory
#undef GetCurrentDirectory
#endif
#ifdef CreateDirectory
#undef CreateDirectory
#endif
#ifdef RemoveDirectory
#undef RemoveDirectory
#endif

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
        static String GetCurrentDirectory();
        static String GetTemporaryDirectory();
        static String GetTemporaryDirectory(PCWSTR);
        static String GetTemporaryDirectory(PCWSTR, PCWSTR);
        static String GetTemporaryDirectory(PCWSTR, PCWSTR, PCWSTR);
        static String GetTemporaryDirectory(PCWSTR, PCWSTR, PCWSTR, PCWSTR);
        static BOOL CreateDirectory(PCWSTR);
        static BOOL RemoveDirectory(PCWSTR);
        static int Compare(PCWSTR psz1, PCWSTR psz2);
        static int Compare(PCSTR psz1, PCSTR psz2);
        static bool WildcardMatch(PCWSTR pszPattern, PCWSTR pszSubject);
        static bool Exists(PCWSTR psz);
        static bool IsFile(PCWSTR psz);
        static bool IsDirectory(PCWSTR psz);
        static bool IsAbsolute(PCWSTR psz);
        static INT64 GetSize(PCWSTR psz);
        static bool ValidateDirectory(PCWSTR psz);
        static Array<DirectoryEntry>& ListFiles(Array<DirectoryEntry>& entries, PCWSTR psz, PCWSTR pszPattern = L"*");
        static Array<DirectoryEntry>& ListDirectories(Array<DirectoryEntry>& entries, PCWSTR psz, PCWSTR pszPattern = L"*");
    };
}
