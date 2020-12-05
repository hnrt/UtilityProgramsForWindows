#include "pch.h"
#include "VersionInfoInternal.h"
#include "hnrt/Win32Exception.h"
#include "hnrt/Heap.h"
#include "hnrt/StringBuffer.h"


#pragma comment(lib,"version")


using namespace hnrt;


RefPtr<VersionInfo> VersionInfo::Create(PCWSTR pszFileName)
{
    return RefPtr<VersionInfo>(new VersionInfoInternal(pszFileName));
}


VersionInfoInternal::VersionInfoInternal(PCWSTR pszFileName)
    : VersionInfo()
    , m_pData(nullptr)
{
    WCHAR szPath[MAX_PATH] = { 0 };
    DWORD dwHandle = 0;
    DWORD dwSize;
    if (!pszFileName)
    {
        if (!GetModuleFileNameW(NULL, szPath, MAX_PATH))
        {
            throw Win32Exception(GetLastError(), L"GetModuleFileName(NULL)");
        }
        pszFileName = szPath;
    }
    dwSize = GetFileVersionInfoSizeW(pszFileName, &dwHandle);
    m_pData = Malloc(dwSize);
    if (!GetFileVersionInfoW(pszFileName, 0, dwSize, m_pData))
    {
        throw Win32Exception(GetLastError(), L"GetFileVersionInfoSize");
    }
    VS_FIXEDFILEINFO* pFFI = nullptr;
    UINT uLen = 0;
    if (!VerQueryValueW(m_pData, L"\\", reinterpret_cast<LPVOID*>(&pFFI), &uLen))
    {
        throw Exception(L"VerQueryValue(\\)");
    }
    m_wFileVersion[0] = HIWORD(pFFI->dwFileVersionMS);
    m_wFileVersion[1] = LOWORD(pFFI->dwFileVersionMS);
    m_wFileVersion[2] = HIWORD(pFFI->dwFileVersionLS);
    m_wFileVersion[3] = LOWORD(pFFI->dwFileVersionLS);
    m_wProductVersion[0] = HIWORD(pFFI->dwProductVersionMS);
    m_wProductVersion[1] = LOWORD(pFFI->dwProductVersionMS);
    m_wProductVersion[2] = HIWORD(pFFI->dwProductVersionLS);
    m_wProductVersion[3] = LOWORD(pFFI->dwProductVersionLS);
    m_dwFileFlagsMask = pFFI->dwFileFlagsMask;
    m_dwFileFlags = pFFI->dwFileFlags;
    m_dwFileOS = pFFI->dwFileOS;
    m_dwFileType = pFFI->dwFileType;
    m_dwFileSubtype = pFFI->dwFileSubtype;
    WORD* pTranslation = nullptr;
    if (!VerQueryValueW(m_pData, L"\\VarFileInfo\\Translation", reinterpret_cast<LPVOID*>(&pTranslation), &uLen))
    {
        throw Exception(L"VerQueryValue(\\VarFileInfo\\Translation)");
    }
    m_cStringFileInfo = static_cast<unsigned long>(uLen / (2 * sizeof(WORD)));
    if (m_cStringFileInfo)
    {
        m_pStringFileInfo = new StringFileInfo[m_cStringFileInfo];
        memset(m_pStringFileInfo, 0, m_cStringFileInfo * sizeof(StringFileInfo));
        for (unsigned long index = 0; index < m_cStringFileInfo; index++)
        {
            m_pStringFileInfo[index].wLanguage = *pTranslation++;
            m_pStringFileInfo[index].wCodePage = *pTranslation++;
#define QS(x) QueryString(m_pStringFileInfo[index].wLanguage, m_pStringFileInfo[index].wCodePage, L#x, m_pStringFileInfo[index].psz##x)
            QS(Comments);
            QS(CompanyName);
            QS(FileDescription);
            QS(FileVersion);
            QS(InternalName);
            QS(LegalCopyright);
            QS(LegalTrademarks);
            QS(OriginalFilename);
            QS(ProductName);
            QS(ProductVersion);
            QS(PrivateBuild);
            QS(SpecialBuild);
#undef QS
        }
    }
}


VersionInfoInternal::~VersionInfoInternal()
{
    free(m_pData);
}


void VersionInfoInternal::QueryString(WORD wLanguage, WORD wCodePage, PCWSTR pszName, wchar_t*& pszValue)
{
    StringBuffer sb(80);
    sb.AppendFormat(L"\\StringFileInfo\\%04x%04x\\%s", wLanguage, wCodePage, pszName);
    UINT uLen = 0;
    if (!VerQueryValueW(m_pData, sb, reinterpret_cast<LPVOID*>(&pszValue), &uLen))
    {
        pszValue = nullptr;
    }
}


bool VersionInfoInternal::Select(unsigned short wLanguage)
{
    if (wLanguage == 0xFFFF)
    {
        wLanguage = GetUserDefaultLangID();
    }
    for (unsigned long index = 0; index < m_cStringFileInfo; index++)
    {
        if (m_pStringFileInfo[index].wLanguage == wLanguage)
        {
            m_dwCurrentIndex = index;
            return true;
        }
    }
    for (unsigned long index = 0; index < m_cStringFileInfo; index++)
    {
        if (LANGID(m_pStringFileInfo[index].wLanguage) == LANGID(wLanguage))
        {
            m_dwCurrentIndex = index;
            return true;
        }
    }
    m_dwCurrentIndex = 0;
    return false;
}
