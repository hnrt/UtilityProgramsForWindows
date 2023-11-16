#include "pch.h"
#include "hnrt/FileWriter.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


FileWriter::FileWriter(PCWSTR pszPath, DWORD dwCreationDisposition)
    : m_Path(pszPath)
    , m_h()
    , m_count(0)
{
    if (m_Path)
    {
        m_h = CreateFileW(pszPath, GENERIC_WRITE, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_h == INVALID_HANDLE_VALUE)
        {
            throw Win32Exception(GetLastError(), L"Failed to open \"%s\".", m_Path.Ptr);
        }
    }
}


void FileWriter::Open(DWORD dwCreationDisposition)
{
    if (!m_Path.Ptr)
    {
        throw Win32Exception(ERROR_INVALID_DATA, L"Failed to open a file due to no path specified.");
    }
    m_h = CreateFileW(m_Path, GENERIC_WRITE, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
    m_count = 0;
    if (m_h == INVALID_HANDLE_VALUE)
    {
        throw Win32Exception(GetLastError(), L"Failed to open \"%s\".", m_Path.Ptr);
    }
}


void FileWriter::Write(LPCVOID pData, SIZE_T cbLen)
{
    const BYTE* pStart = reinterpret_cast<const BYTE*>(pData);
    const BYTE* pEnd = pStart + cbLen;
    const BYTE* pCur = pStart;
    while (pCur < pEnd)
    {
        DWORD dwByteCount = 0;
        if (WriteFile(m_h, pCur, static_cast<DWORD>(pEnd - pCur), &dwByteCount, NULL))
        {
            pCur += dwByteCount;
        }
        else
        {
            m_count += pCur - pStart;
            throw Win32Exception(GetLastError(), L"Failed to write to \"%s\".", m_Path.Ptr);
        }
    }
    m_count += pCur - pStart;
}
