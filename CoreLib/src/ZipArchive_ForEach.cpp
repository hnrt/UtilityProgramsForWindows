#include "pch.h"
#include "hnrt/ZipArchive.h"
#include "hnrt/ComException.h"
#include "hnrt/Variant.h"


using namespace hnrt;


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

    LONG count = 0L;
    hr = ctx.pFolderItems->get_Count(&count);
    if (hr != S_OK)
    {
        throw ComException(hr, L"FolderItems::get_Count failed.");
    }
 
    for (LONG index = 0L; index < count; index++)
    {
        FolderItem* pFolderItem;
        hr = ctx.pFolderItems->Item(Variant(VT_I4, index), &pFolderItem);
        if (hr != S_OK)
        {
            throw ComException(hr, L"FolderItems::Item failed.");
        }
        ZipArchiveEntry entry(pFolderItem);
        callback.ZipForEach(entry);
    }
}
