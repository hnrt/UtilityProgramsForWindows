#include "pch.h"
#include "hnrt/ZipArchiveEntry.h"
#include "hnrt/ComException.h"
#include "hnrt/Variant.h"


using namespace hnrt;


void ZipArchiveEntry::ForEach(IZipForEachCallback& callback)
{
    if (!m_bIsFolder)
    {
        return;
    }

    struct ForEachLocalContext
    {
        Folder* pFolder;
        FolderItems* pFolderItems;

        ForEachLocalContext()
            : pFolder(nullptr)
            , pFolderItems(nullptr)
        {
        }

        ~ForEachLocalContext()
        {
            if (pFolder)
            {
                pFolder->Release();
            }
            if (pFolderItems)
            {
                pFolderItems->Release();
            }
        }

    } ctx;

    IDispatch* pDispatch;
    HRESULT hr = m_pFolderItem->get_GetFolder(&pDispatch);
    if (hr != S_OK)
    {
        throw ComException(hr, L"GetFolder failed.");
    }
    ctx.pFolder = static_cast<Folder*>(pDispatch);

    hr = ctx.pFolder->Items(&ctx.pFolderItems);
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
