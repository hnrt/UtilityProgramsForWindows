#include "pch.h"
#include "hnrt/ZipArchive.h"
#include "hnrt/ComException.h"


using namespace hnrt;


ZipArchive::~ZipArchive()
{
    m_pFolder->Release();
}


ZipArchive::ZipArchive(const String& szFileName, Folder* pFolder)
    : m_szFileName(szFileName)
    , m_pFolder(pFolder)
{
}


ZipArchive::ZipArchive(const ZipArchive& src)
    : m_szFileName(src.m_szFileName)
    , m_pFolder(src.m_pFolder)
{
    m_pFolder->AddRef();
}


ZipArchive& ZipArchive::operator =(const ZipArchive& src)
{
    m_szFileName = src.m_szFileName;
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
