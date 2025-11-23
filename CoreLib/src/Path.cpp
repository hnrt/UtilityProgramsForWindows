#include "pch.h"
#include <string.h>
#include <cctype>
#include <ShlObj_core.h>
#include <shtypes.h>
#include <Windows.h>
#include "hnrt/Path.h"
#include "hnrt/String.h"
#include "hnrt/StringCaseInsensitive.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/ComException.h"
#include "hnrt/WindowsHandle.h"
#include "hnrt/Exception.h"
#include "hnrt/Array.h"
#include "hnrt/DirectoryEntry.h"


using namespace hnrt;


constexpr auto DRIVE_LETTER_SEPARATOR_CHAR = L':';
constexpr auto DIRECTORY_SEPARATOR_CHAR = L'\\';
constexpr auto EXTENSION_LEADER_CHAR = L'.';


const WCHAR Path::DirectorySeparatorChar = DIRECTORY_SEPARATOR_CHAR;
const WCHAR Path::DirectorySeparator[2] = { DIRECTORY_SEPARATOR_CHAR, L'\0' };


String Path::GetFileName(PCWSTR psz)
{
    PCWSTR pLastSeparator = wcsrchr(psz, DIRECTORY_SEPARATOR_CHAR);
    PCWSTR pszFileName = pLastSeparator ? pLastSeparator + 1 : psz;
    return String(pszFileName);
}


String Path::GetFileNameWithoutExtension(PCWSTR psz)
{
    PCWSTR pLastSeparator = wcsrchr(psz, DIRECTORY_SEPARATOR_CHAR);
    PCWSTR pszFileName = pLastSeparator ? pLastSeparator + 1 : psz;
    PCWSTR pExtension = wcsrchr(pszFileName, EXTENSION_LEADER_CHAR);
    size_t cch = (pExtension ? pExtension : wcsrchr(pszFileName, L'\0')) - pszFileName;
    return String(pszFileName, cch);
}


String Path::GetExtension(PCWSTR psz)
{
    PCWSTR pszExtension = wcsrchr(wcschr(psz, DIRECTORY_SEPARATOR_CHAR) ? wcsrchr(psz, DIRECTORY_SEPARATOR_CHAR) + 1 : psz, EXTENSION_LEADER_CHAR);
    return String(pszExtension ? pszExtension : L"");
}


String Path::GetDirectoryName(PCWSTR psz, bool bEndsWithSeparator)
{
    PCWSTR p1;
    PCWSTR p2;
    if (psz[0] == DIRECTORY_SEPARATOR_CHAR && psz[1] == DIRECTORY_SEPARATOR_CHAR)
    {
        // UNC ?
        if (!psz[2] || psz[2] == DIRECTORY_SEPARATOR_CHAR)
        {
            return String::Empty;
        }
        PCWSTR p = wcschr(psz + 3, DIRECTORY_SEPARATOR_CHAR);
        if (!p || !p[1] || p[1] == DIRECTORY_SEPARATOR_CHAR)
        {
            return String::Empty;
        }
        p = wcschr(p + 2, DIRECTORY_SEPARATOR_CHAR);
        if (!p)
        {
            return bEndsWithSeparator ? String(psz, Path::DirectorySeparator) : String(psz);
        }
        p1 = p;
        p2 = wcschr(p + 1, DIRECTORY_SEPARATOR_CHAR);
    }
    else if (iswalpha(psz[0]) && psz[1] == DRIVE_LETTER_SEPARATOR_CHAR && psz[2] == DIRECTORY_SEPARATOR_CHAR)
    {
        // DOS absolute with drive letter ?
        p1 = psz + (bEndsWithSeparator ? 2 : 3);
        p2 = wcschr(psz + 3, DIRECTORY_SEPARATOR_CHAR);
    }
    else if (psz[0] == DIRECTORY_SEPARATOR_CHAR)
    {
        // DOS absolute without drive letter ?
        p1 = psz + (bEndsWithSeparator ? 0 : 1);
        p2 = wcschr(psz + 1, DIRECTORY_SEPARATOR_CHAR);
    }
    else if (psz[0] == L'.' && (psz[1] == L'\0' || (psz[1] == DIRECTORY_SEPARATOR_CHAR && psz[2] == L'\0')))
    {
        return bEndsWithSeparator ? String(L"..", Path::DirectorySeparator) : String(L"..");
    }
    else
    {
        p1 = nullptr;
        p2 = wcschr(psz, DIRECTORY_SEPARATOR_CHAR);
    }
    while (p2)
    {
        if (!p2[1])
        {
            break;
        }
        p1 = p2;
        p2 = wcschr(p2 + 1, DIRECTORY_SEPARATOR_CHAR);
    }
    return p1 ?
        String(psz, p1 + (bEndsWithSeparator ? 1 : 0) - psz) :
        bEndsWithSeparator ? String(L".", Path::DirectorySeparator) : String(L".");
}


inline static void CombineEntry(StringBuffer& buf, PCWSTR psz)
{
    if (buf[buf.Len - 1] != DIRECTORY_SEPARATOR_CHAR)
    {
        buf.Append(Path::DirectorySeparator);
    }
    buf.Append(psz);
}


inline static void CombineEntry(StringBuffer& buf, PCWSTR psz, INT_PTR cch)
{
    if (buf[buf.Len - 1] != DIRECTORY_SEPARATOR_CHAR)
    {
        buf.Append(Path::DirectorySeparator);
    }
    buf.Append(psz, cch);
}


String Path::Combine(PCWSTR psz1, PCWSTR psz2, INT_PTR cch2)
{
    if (!psz1 || !*psz1)
    {
        throw Exception(L"Path::Combine: Arg#1 is empty.");
    }
    if (!psz2 || !*psz2 || !cch2)
    {
        throw Exception(L"Path::Combine: Arg#2 is empty.");
    }
    StringBuffer buf(MAX_PATH, psz1);
    CombineEntry(buf, psz2, cch2);
    return String(buf);
}


String Path::Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, INT_PTR cch3)
{
    if (!psz1 || !*psz1)
    {
        throw Exception(L"Path::Combine: Arg#1 is empty.");
    }
    if (!psz2 || !*psz2)
    {
        throw Exception(L"Path::Combine: Arg#2 is empty.");
    }
    if (!psz3 || !*psz3 || !cch3)
    {
        throw Exception(L"Path::Combine: Arg#3 is empty.");
    }
    StringBuffer buf(MAX_PATH, psz1);
    CombineEntry(buf, psz2);
    CombineEntry(buf, psz3, cch3);
    return String(buf);
}


String Path::Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, INT_PTR cch4)
{
    if (!psz1 || !*psz1)
    {
        throw Exception(L"Path::Combine: Arg#1 is empty.");
    }
    if (!psz2 || !*psz2)
    {
        throw Exception(L"Path::Combine: Arg#2 is empty.");
    }
    if (!psz3 || !*psz3)
    {
        throw Exception(L"Path::Combine: Arg#3 is empty.");
    }
    if (!psz4 || !*psz4 || !cch4)
    {
        throw Exception(L"Path::Combine: Arg#4 is empty.");
    }
    StringBuffer buf(MAX_PATH, psz1);
    CombineEntry(buf, psz2);
    CombineEntry(buf, psz3);
    CombineEntry(buf, psz4, cch4);
    return String(buf);
}


String Path::Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5, INT_PTR cch5)
{
    if (!psz1 || !*psz1)
    {
        throw Exception(L"Path::Combine: Arg#1 is empty.");
    }
    if (!psz2 || !*psz2)
    {
        throw Exception(L"Path::Combine: Arg#2 is empty.");
    }
    if (!psz3 || !*psz3)
    {
        throw Exception(L"Path::Combine: Arg#3 is empty.");
    }
    if (!psz4 || !*psz4)
    {
        throw Exception(L"Path::Combine: Arg#4 is empty.");
    }
    if (!psz5 || !*psz5 || !cch5)
    {
        throw Exception(L"Path::Combine: Arg#5 is empty.");
    }
    StringBuffer buf(MAX_PATH, psz1);
    CombineEntry(buf, psz2);
    CombineEntry(buf, psz3);
    CombineEntry(buf, psz4);
    CombineEntry(buf, psz5, cch5);
    return String(buf);
}


String Path::GetKnownFolder(const KNOWNFOLDERID& rid, DWORD dwFlags)
{
    PWSTR psz;
    HRESULT hr = SHGetKnownFolderPath(rid, dwFlags, NULL, &psz);
    if (hr == S_OK)
    {
        String s(psz);
        CoTaskMemFree(psz);
        return s;
    }
    else
    {
        throw ComException(hr, L"KnownFolderPath::ctor failed.");
    }
}


String Path::GetCurrentDirectory()
{
    WCHAR szPath[MAX_PATH] = { 0 };
    if (GetCurrentDirectoryW(MAX_PATH, szPath))
    {
        return String(szPath);
    }
    else
    {
        return String(L".");
    }
}


int Path::Compare(PCWSTR psz1, PCWSTR psz2)
{
    return StringCase::Compare(psz1, -1, psz2, -1);
}


int Path::Compare(PCSTR psz1, PCSTR psz2)
{
    return StringCase::Compare(String(CP_ACP, psz1), -1, String(CP_ACP, psz2), -1);
}


bool Path::WildcardMatch(PCWSTR pszPattern, PCWSTR pszSubject)
{
    const WCHAR* p1 = pszPattern;
    const WCHAR* p2 = pszSubject;
    while (true)
    {
        if (!*p1)
        {
            return !*p2;
        }
        else if (*p1 == L'*')
        {
            ++p1;
            while (*p1 == L'*')
            {
                ++p1;
            }
            if (!*p1)
            {
                return true;
            }
            else if (!*p2)
            {
                return false;
            }
            while (true)
            {
                if (WildcardMatch(p1, p2))
                {
                    return true;
                }
                ++p2;
                if (!*p2)
                {
                    return false;
                }
            }
            //NEVER REACH HERE
        }
        else if (!*p2)
        {
            return false;
        }
        else if (*p1 == L'?')
        {
            p1++;
            p2++;
        }
        else if (towupper(*p1) == towupper(*p2))
        {
            p1++;
            p2++;
        }
        else
        {
            return false;
        }
    }
}


bool Path::Exists(PCWSTR psz)
{
    DWORD dw = GetFileAttributesW(psz);
    return dw != INVALID_FILE_ATTRIBUTES;
}


bool Path::IsFile(PCWSTR psz)
{
    DWORD dw = GetFileAttributesW(psz);
    return dw != INVALID_FILE_ATTRIBUTES && (dw & FILE_ATTRIBUTE_NORMAL) != 0;
}


bool Path::IsDirectory(PCWSTR psz)
{
    DWORD dw = GetFileAttributesW(psz);
    return dw != INVALID_FILE_ATTRIBUTES && (dw & FILE_ATTRIBUTE_DIRECTORY) != 0;
}


bool Path::IsAbsolute(PCWSTR psz)
{
    return (iswalpha(psz[0]) && psz[1] == DRIVE_LETTER_SEPARATOR_CHAR && psz[2] == DIRECTORY_SEPARATOR_CHAR) || (psz[0] == DIRECTORY_SEPARATOR_CHAR);
}


INT64 Path::GetSize(PCWSTR psz)
{
    LARGE_INTEGER size = { 0 };
    size.QuadPart = -1LL;
    WindowsHandle h = CreateFileW(psz, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE)
    {
        GetFileSizeEx(h, &size);
    }
    return size.QuadPart;
}


bool Path::ValidateDirectory(PCWSTR psz)
{
    if (Path::IsDirectory(psz))
    {
        return true;
    }
    else if (ValidateDirectory(Path::GetDirectoryName(psz)))
    {
        if (CreateDirectoryW(psz, NULL))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}


Array<DirectoryEntry>& Path::ListFiles(Array<DirectoryEntry>& entries, PCWSTR psz, PCWSTR pszPattern)
{
    WIN32_FIND_DATAW fd = { 0 };
    HANDLE h = FindFirstFileW(Path::Combine(psz, pszPattern), &fd);
    if (h != INVALID_HANDLE_VALUE)
    {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            entries.PushBack(DirectoryEntry(fd));
        }
        while (FindNextFileW(h, &fd))
        {
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
            {
                entries.PushBack(DirectoryEntry(fd));
            }
        }
        FindClose(h);
    }
    return entries;
}


inline static bool IsCurrentOrParentDirectory(PCWSTR psz)
{
    return psz[0] == L'.' && (psz[1] == L'\0' || (psz[1] == L'.' && psz[2] == L'\0'));
}


Array<DirectoryEntry>& Path::ListDirectories(Array<DirectoryEntry>& entries, PCWSTR psz, PCWSTR pszPattern)
{
    WIN32_FIND_DATAW fd = { 0 };
    HANDLE h = FindFirstFileW(Path::Combine(psz, pszPattern), &fd);
    if (h != INVALID_HANDLE_VALUE)
    {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
        {
            if (!IsCurrentOrParentDirectory(fd.cFileName))
            {
                entries.PushBack(DirectoryEntry(fd));
            }
        }
        while (FindNextFileW(h, &fd))
        {
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
            {
                if (!IsCurrentOrParentDirectory(fd.cFileName))
                {
                    entries.PushBack(DirectoryEntry(fd));
                }
            }
        }
        FindClose(h);
    }
    return entries;
}
