#include "pch.h"
#include "hnrt/FileMapper.h"
#include "hnrt/String.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


FileMapper::FileMapper(PCWSTR pszPath)
    : m_pszPath(NULL)
    , m_hFile(INVALID_HANDLE_VALUE)
    , m_hFileMapping(NULL)
    , m_ptr(NULL)
    , m_len(0ULL)
{
    if (pszPath)
    {
        m_pszPath = String::Copy(pszPath);
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
    if (!m_pszPath)
    {
        throw Win32Exception(ERROR_INVALID_DATA, L"Failed to open a file due to no path specified.");
    }
    m_hFile = CreateFileW(m_pszPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        DWORD dwError = GetLastError();
        throw Win32Exception(dwError, L"Failed to open \"%s\".", m_pszPath);
    }
    LARGE_INTEGER size;
    size.QuadPart = -1LL;
    if (!GetFileSizeEx(m_hFile, &size))
    {
        DWORD dwError = GetLastError();
        Close();
        throw Win32Exception(dwError, L"Failed to get size of \"%s\".", m_pszPath);
    }
    if (size.QuadPart <= 0LL)
    {
        return;
    }
    m_hFileMapping = CreateFileMappingW(m_hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (!m_hFileMapping)
    {
        DWORD dwError = GetLastError();
        Close();
        throw Win32Exception(dwError, L"Failed to create a mapping to \"%s\".", m_pszPath);
    }
    m_ptr = MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if (!m_ptr)
    {
        DWORD dwError = GetLastError();
        Close();
        throw Win32Exception(dwError, L"Failed to map a view of \"%s\".", m_pszPath);
    }
    m_len = size.QuadPart;
}


void FileMapper::Close()
{
    if (m_ptr)
    {
        UnmapViewOfFile(m_ptr);
        m_ptr = NULL;
        m_len = 0ULL;
    }
    if (m_hFileMapping)
    {
        CloseHandle(m_hFileMapping);
        m_hFileMapping = NULL;
    }
    if (m_hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
}


void FileMapper::set_Path(PCWSTR pszPath)
{
    m_pszPath = pszPath ? String::Copy(pszPath) : nullptr;
}
