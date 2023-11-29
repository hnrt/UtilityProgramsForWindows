#include "pch.h"
#include "hnrt/Path.h"
#include "hnrt/ComException.h"
#include "hnrt/ZipArchive.h"
#include "hnrt/ZipArchiveEntry.h"
#include "ZipInternal.h"


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


ZipArchiveEntry& ZipArchiveEntry::operator =(const ZipArchiveEntry& src)
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


void ZipArchiveEntry::ExtractTo(PCWSTR pszOutFolder, IZipExtractCallbacks& callbacks)
{
    if (m_bIsFolder)
    {
        struct ExtractToLocalContextForFolder
            : public IZipForEachCallback
        {
            PCWSTR pszOutFolder;
            IZipExtractCallbacks& callbacks;

            ExtractToLocalContextForFolder(PCWSTR pszOutFolder_, IZipExtractCallbacks& callbacks_)
                : pszOutFolder(pszOutFolder_)
                , callbacks(callbacks_)
            {
            }

            virtual void ZipForEach(ZipArchiveEntry& entry)
            {
                entry.ExtractTo(pszOutFolder, callbacks);
            }

        } ctx(pszOutFolder, callbacks);

        ForEach(ctx);
    }
    else
    {
        struct ExtractToLocalContext
        {
            Folder* pFolder;

            ExtractToLocalContext(PCWSTR pszFolder)
                : pFolder(ZipInternal::GetFolder(pszFolder))
            {
            }

            ~ExtractToLocalContext()
            {
                pFolder->Release();
            }

        } ctx(pszOutFolder);

        PCWSTR pszPath = Path::Combine(pszOutFolder, m_bstrName);
        callbacks.ZipExtractOnStart(pszPath, m_Size);
        VARIANT item, options;
        VariantInit(&item);
        item.vt = VT_DISPATCH;
        item.pdispVal = m_pFolderItem;
        VariantInit(&options);
        options.vt = VT_I4;
        options.lVal = FOF_NO_UI;
        HRESULT hr = ctx.pFolder->CopyHere(item, options);
        if (hr == S_OK)
        {
            callbacks.ZipExtractOnSuccess(pszPath);
        }
        else
        {
            callbacks.ZipExtractOnFailure(pszPath, hr);
        }
    }
}
