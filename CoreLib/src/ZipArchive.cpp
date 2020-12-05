#include "pch.h"
#include "hnrt/String.h"
#include "hnrt/ComException.h"
#include "hnrt/ZipArchive.h"


using namespace hnrt;


ZipArchive::~ZipArchive()
{
    m_pFolder->Release();
}


ZipArchive::ZipArchive(PCWSTR pszFileName, Folder* pFolder)
    : m_pszFileName(String::Copy(pszFileName))
    , m_pFolder(pFolder)
{
}


ZipArchive::ZipArchive(const ZipArchive& src)
    : m_pszFileName(src.m_pszFileName)
    , m_pFolder(src.m_pFolder)
{
    m_pFolder->AddRef();
}


ZipArchive& ZipArchive::operator =(const ZipArchive& src)
{
    m_pszFileName = src.m_pszFileName;
    if (m_pFolder)
    {
        m_pFolder->Release();
    }
    m_pFolder = src.m_pFolder;
    m_pFolder->AddRef();
    return *this;
}


void ZipArchive::ForEach(IZipForEachCallback& callback)
{
    struct LocalContext
    {
        FolderItems* pFolderItems;

        LocalContext()
            : pFolderItems(nullptr)
        {
        }

        ~LocalContext()
        {
            if (pFolderItems)
            {
                pFolderItems->Release();
            }
        }

    } ctx;

    HRESULT hr = m_pFolder->Items(&ctx.pFolderItems);
    if (hr != S_OK)
    {
        throw ComException(hr, L"Folder::Items failed.");
    }

    LONG count = 0;
    hr = ctx.pFolderItems->get_Count(&count);
    if (hr != S_OK)
    {
        throw ComException(hr, L"FolderItems::get_Count failed.");
    }
 
    VARIANT varIndex;
    VariantInit(&varIndex);
    varIndex.vt = VT_I4;
    for (varIndex.lVal = 0; varIndex.lVal < count; varIndex.lVal++)
    {
        FolderItem* pFolderItem;
        hr = ctx.pFolderItems->Item(varIndex, &pFolderItem);
        if (hr != S_OK)
        {
            throw ComException(hr, L"FolderItems::Item failed.");
        }
        ZipArchiveEntry entry(pFolderItem);
        callback.ZipForEach(entry);
    }
}
