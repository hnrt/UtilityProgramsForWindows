#include "pch.h"
#include <ctype.h>
#include "hnrt/Path.h"
#include "hnrt/String.h"
#include "hnrt/StringBuffer.h"
#include "hnrt/ComException.h"


using namespace hnrt;


#define DRIVE_LETTER_SEPARATOR_CHAR L':'
#define DIRECTORY_SEPARATOR_CHAR L'\\'
#define EXTENSION_LEADER_CHAR L'.'


const wchar_t Path::DirectorySeparatorChar = DIRECTORY_SEPARATOR_CHAR;
PCWSTR Path::DirectorySeparator = L"\\";


PCWSTR Path::GetFileName(PCWSTR psz)
{
    PCWSTR pLastSeparator = wcsrchr(psz, DIRECTORY_SEPARATOR_CHAR);
    PCWSTR pszFileName = pLastSeparator ? pLastSeparator + 1 : psz;
    return String::Copy(pszFileName);
}


PCWSTR Path::GetFileNameWithoutExtension(PCWSTR psz)
{
    PCWSTR pLastSeparator = wcsrchr(psz, DIRECTORY_SEPARATOR_CHAR);
    PCWSTR pszFileName = pLastSeparator ? pLastSeparator + 1 : psz;
    PCWSTR pExtension = wcsrchr(pszFileName, EXTENSION_LEADER_CHAR);
    size_t cch = (pExtension ? pExtension : wcsrchr(pszFileName, L'\0')) - pszFileName;
    return String::Copy(pszFileName, cch);
}


PCWSTR Path::GetExtension(PCWSTR psz)
{
    PCWSTR pszExtension = wcsrchr(wcschr(psz, DIRECTORY_SEPARATOR_CHAR) ? wcsrchr(psz, DIRECTORY_SEPARATOR_CHAR) + 1 : psz, EXTENSION_LEADER_CHAR);
    return String::Copy(pszExtension ? pszExtension : L"");
}


PCWSTR Path::GetDirectoryName(PCWSTR psz, bool bEndsWithSeparator)
{
    PCWSTR p1;
    PCWSTR p2;
    if (psz[0] == DIRECTORY_SEPARATOR_CHAR && psz[1] == DIRECTORY_SEPARATOR_CHAR)
    {
        // UNC ?
        if (!psz[2] || psz[2] == DIRECTORY_SEPARATOR_CHAR)
        {
            return nullptr;
        }
        PCWSTR p = wcschr(psz + 3, DIRECTORY_SEPARATOR_CHAR);
        if (!p || !p[1] || p[1] == DIRECTORY_SEPARATOR_CHAR)
        {
            return nullptr;
        }
        p = wcschr(p + 2, DIRECTORY_SEPARATOR_CHAR);
        if (!p)
        {
            return String::Format(bEndsWithSeparator ? L"%s\\" : L"%s", psz);
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
        return String::Copy(bEndsWithSeparator ? L"..\\" : L"..");
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
    return p1 ? String::Copy(psz, p1 + (bEndsWithSeparator ? 1 : 0) - psz) : String::Copy(bEndsWithSeparator ? L".\\" : L".");
}


PCWSTR Path::Combine(PCWSTR psz1, PCWSTR psz2, size_t cch2)
{
    if (!psz1 || !*psz1)
    {
        throw Exception(L"Path::Combine: Arg#1 is empty.");
    }
    if (!psz2 || !*psz2 || !cch2)
    {
        throw Exception(L"Path::Combine: Arg#2 is empty.");
    }
    if (cch2 == static_cast<size_t>(-1))
    {
        cch2 = wcslen(psz2);
    }
    StringBuffer buf(MAX_PATH);
    buf.AppendFormat(L"%s", psz1);
    buf.AppendFormat(buf[buf.Len - 1] == DIRECTORY_SEPARATOR_CHAR ? L"%.*s" : L"\\%.*s", static_cast<int>(cch2), psz2);
    return String::Copy(buf);
}


PCWSTR Path::Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, size_t cch3)
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
    if (cch3 == static_cast<size_t>(-1))
    {
        cch3 = wcslen(psz3);
    }
    StringBuffer buf(MAX_PATH);
    buf.AppendFormat(L"%s", psz1);
    buf.AppendFormat(buf[buf.Len - 1] == DIRECTORY_SEPARATOR_CHAR ? L"%s" : L"\\%s", psz2);
    buf.AppendFormat(buf[buf.Len - 1] == DIRECTORY_SEPARATOR_CHAR ? L"%.*s" : L"\\%.*s", static_cast<int>(cch3), psz3);
    return String::Copy(buf);
}


PCWSTR Path::Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, size_t cch4)
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
    if (cch4 == static_cast<size_t>(-1))
    {
        cch4 = wcslen(psz4);
    }
    StringBuffer buf(MAX_PATH);
    buf.AppendFormat(L"%s", psz1);
    buf.AppendFormat(buf[buf.Len - 1] == DIRECTORY_SEPARATOR_CHAR ? L"%s" : L"\\%s", psz2);
    buf.AppendFormat(buf[buf.Len - 1] == DIRECTORY_SEPARATOR_CHAR ? L"%s" : L"\\%s", psz3);
    buf.AppendFormat(buf[buf.Len - 1] == DIRECTORY_SEPARATOR_CHAR ? L"%.*s" : L"\\%.*s", static_cast<int>(cch4), psz4);
    return String::Copy(buf);
}


PCWSTR Path::Combine(PCWSTR psz1, PCWSTR psz2, PCWSTR psz3, PCWSTR psz4, PCWSTR psz5, size_t cch5)
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
    if (cch5 == static_cast<size_t>(-1))
    {
        cch5 = wcslen(psz5);
    }
    StringBuffer buf(MAX_PATH);
    buf.AppendFormat(L"%s", psz1);
    buf.AppendFormat(buf[buf.Len - 1] == DIRECTORY_SEPARATOR_CHAR ? L"%s" : L"\\%s", psz2);
    buf.AppendFormat(buf[buf.Len - 1] == DIRECTORY_SEPARATOR_CHAR ? L"%s" : L"\\%s", psz3);
    buf.AppendFormat(buf[buf.Len - 1] == DIRECTORY_SEPARATOR_CHAR ? L"%s" : L"\\%s", psz4);
    buf.AppendFormat(buf[buf.Len - 1] == DIRECTORY_SEPARATOR_CHAR ? L"%.*s" : L"\\%.*s", static_cast<int>(cch5), psz5);
    return String::Copy(buf);
}


PCWSTR Path::GetKnownFolder(const KNOWNFOLDERID& rid, DWORD dwFlags)
{
    PWSTR psz;
    HRESULT hr = SHGetKnownFolderPath(rid, dwFlags, NULL, &psz);
    if (hr == S_OK)
    {
        PCWSTR psz2 = String::Copy(psz);
        CoTaskMemFree(psz);
        return psz2;
    }
    else
    {
        throw ComException(hr, L"KnownFolderPath::ctor failed.");
    }
}


int Path::Compare(PCWSTR psz1, PCWSTR psz2)
{
    return String::CaseCompare(psz1, psz2);
}


int Path::Compare(const char* psz1, const char* psz2)
{
    return String::CaseCompare(psz1, psz2);
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
    LARGE_INTEGER size;
    size.QuadPart = -1LL;
    HANDLE h = CreateFileW(psz, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE)
    {
        GetFileSizeEx(h, &size);
        CloseHandle(h);
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
