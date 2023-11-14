#include "pch.h"
#include "hnrt/FileMapper.h"
#include "hnrt/Win32Exception.h"
#include <Intrin.h>


using namespace hnrt;


FileMapper::FileMapper(PCWSTR pszPath)
    : m_Path(pszPath)
    , m_hFile()
    , m_hFileMapping()
    , m_ptr(NULL)
    , m_len(0ULL)
{
    if (m_Path.Ptr)
    {
        Open();
    }
}


FileMapper::~FileMapper()
{
    Close();
}


void FileMapper::Open()
{
    Close();
    if (!m_Path.Ptr)
    {
        throw Win32Exception(ERROR_INVALID_DATA, L"Failed to open a file due to no path specified.");
    }
    m_hFile = CreateFileW(m_Path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        DWORD dwError = GetLastError();
        throw Win32Exception(dwError, L"Failed to open \"%s\".", m_Path.Str);
    }
    LARGE_INTEGER size = { 0 };
    size.QuadPart = -1LL;
    if (!GetFileSizeEx(m_hFile, &size))
    {
        DWORD dwError = GetLastError();
        throw Win32Exception(dwError, L"Failed to get size of \"%s\".", m_Path.Str);
    }
    if (size.QuadPart <= 0LL)
    {
        return;
    }
    m_hFileMapping = CreateFileMappingW(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!m_hFileMapping)
    {
        DWORD dwError = GetLastError();
        throw Win32Exception(dwError, L"Failed to create a mapping to \"%s\".", m_Path.Str);
    }
    m_ptr = MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if (!m_ptr)
    {
        DWORD dwError = GetLastError();
        throw Win32Exception(dwError, L"Failed to map a view of \"%s\".", m_Path.Str);
    }
    m_len = size.QuadPart;
}


void FileMapper::Close()
{
    PVOID ptr = _InterlockedExchangePointer(&m_ptr, NULL);
    if (ptr)
    {
        UnmapViewOfFile(ptr);
    }
    m_len = 0ULL;
    m_hFileMapping = NULL;
    m_hFile = INVALID_HANDLE_VALUE;
}
