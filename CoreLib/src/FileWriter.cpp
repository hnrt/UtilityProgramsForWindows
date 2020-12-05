#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/FileWriter.h"


using namespace hnrt;


FileWriter::FileWriter(PCWSTR pszPath, DWORD dwCreationDisposition)
    : m_pszPath(nullptr)
    , m_h(INVALID_HANDLE_VALUE)
    , m_count(0)
{
    if (pszPath)
    {
        m_pszPath = String::Copy(pszPath);
        m_h = CreateFileW(pszPath, GENERIC_WRITE, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_h == INVALID_HANDLE_VALUE)
        {
            throw Win32Exception(GetLastError(), L"Failed to open \"%s\".", m_pszPath);
        }
    }
}


FileWriter::~FileWriter()
{
    if (m_h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_h);
    }
}


void FileWriter::Open(DWORD dwCreationDisposition)
{
    if (m_h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_h);
        m_h = INVALID_HANDLE_VALUE;
        m_count = 0;
    }
    if (!m_pszPath)
    {
        throw Win32Exception(ERROR_INVALID_DATA, L"Failed to open a file due to no path specified.");
    }
    m_h = CreateFileW(m_pszPath, GENERIC_WRITE, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_h == INVALID_HANDLE_VALUE)
    {
        throw Win32Exception(GetLastError(), L"Failed to open \"%s\".", m_pszPath);
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
            throw Win32Exception(GetLastError(), L"Failed to write to \"%s\".", m_pszPath);
        }
    }
    m_count += pCur - pStart;
}


void FileWriter::Close()
{
    if (m_h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_h);
        m_h = INVALID_HANDLE_VALUE;
        m_count = 0;
    }
}


void FileWriter::set_Path(PCWSTR pszPath)
{
    m_pszPath = pszPath ? String::Copy(pszPath) : nullptr;
}
