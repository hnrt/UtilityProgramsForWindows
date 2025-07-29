#include "pch.h"
#include "hnrt/ZipArchiveEntry.h"
#include "hnrt/ComException.h"


using namespace hnrt;


ZipArchiveEntry::ZipArchiveEntry()
    : m_pFolderItem(nullptr)
    , m_bstrName(nullptr)
    , m_bstrType(nullptr)
    , m_Size(0)
    , m_bIsFolder(FALSE)
{
}


ZipArchiveEntry::ZipArchiveEntry(FolderItem* pFolderItem)
    : m_pFolderItem(pFolderItem)
    , m_bstrName(nullptr)
    , m_bstrType(nullptr)
    , m_Size(0)
    , m_bIsFolder(FALSE)
{
    HRESULT hr;
    hr = m_pFolderItem->get_Name(&m_bstrName);
    if (hr != S_OK) throw ComException(hr, L"ZipArchiveEntry::ctor: get_Name failed.");
    hr = m_pFolderItem->get_Type(&m_bstrType);
    if (hr != S_OK) throw ComException(hr, L"ZipArchiveEntry::ctor: get_Type failed.");
    hr = m_pFolderItem->get_Size(&m_Size);
    if (hr != S_OK) throw ComException(hr, L"ZipArchiveEntry::ctor: get_Size failed.");
    hr = m_pFolderItem->get_IsFolder(&m_bIsFolder);
    if (hr != S_OK) throw ComException(hr, L"ZipArchiveEntry::ctor: get_IsFolder failed.");
}


ZipArchiveEntry::ZipArchiveEntry(const ZipArchiveEntry& src)
    : m_pFolderItem(src.m_pFolderItem)
    , m_bstrName(SysAllocString(src.m_bstrName))
    , m_bstrType(SysAllocString(src.m_bstrType))
    , m_Size(src.m_Size)
    , m_bIsFolder(src.m_bIsFolder)
{
    m_pFolderItem->AddRef();
}
