#include "pch.h"
#include "hnrt/FileWriter.h"
#include "hnrt/Win32Exception.h"


using namespace hnrt;


FileWriter::FileWriter(const String& szPath, DWORD dwCreationDisposition)
    : m_szPath(szPath)
    , m_h()
    , m_count(0)
{
    if (m_szPath.Len)
    {
        m_h = CreateFileW(m_szPath, GENERIC_WRITE, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_h == INVALID_HANDLE_VALUE)
        {
            throw Win32Exception(GetLastError(), L"Failed to open \"%s\".", m_szPath);
        }
    }
}


void FileWriter::Open(DWORD dwCreationDisposition)
{
    if (!m_szPath.Len)
    {
        throw Win32Exception(ERROR_INVALID_DATA, L"Failed to open a file due to no path specified.");
    }
    m_h = CreateFileW(m_szPath, GENERIC_WRITE, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
    m_count = 0;
    if (m_h == INVALID_HANDLE_VALUE)
    {
        throw Win32Exception(GetLastError(), L"Failed to open \"%s\".", m_szPath);
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
            throw Win32Exception(GetLastError(), L"Failed to write to \"%s\".", m_szPath);
        }
    }
    m_count += pCur - pStart;
}
