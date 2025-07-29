#include "pch.h"
#include "hnrt/ZipArchiveEntry.h"


using namespace hnrt;


ZipArchiveEntry::~ZipArchiveEntry()
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
}
