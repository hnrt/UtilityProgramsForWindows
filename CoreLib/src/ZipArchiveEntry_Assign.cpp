#include "pch.h"
#include "hnrt/ZipArchiveEntry.h"


using namespace hnrt;


ZipArchiveEntry& ZipArchiveEntry::Assign(const ZipArchiveEntry& src)
{
    if (m_pFolderItem)
    {
        m_pFolderItem->Release();
    }
    if (m_bstrName)
    {
        SysFreeString(m_bstrName);
    }
    if (m_bstrType)
    {
        SysFreeString(m_bstrType);
    }
    m_pFolderItem = src.m_pFolderItem;
    m_pFolderItem->AddRef();
    m_bstrName = SysAllocString(src.m_bstrName);
    m_bstrType = SysAllocString(src.m_bstrType);
    m_Size = src.m_Size;
    m_bIsFolder = src.m_bIsFolder;
    return *this;
}
