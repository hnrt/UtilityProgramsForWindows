#include "pch.h"
#include "hnrt/ZipArchiveEntry.h"
#include "hnrt/Path.h"
#include "hnrt/Variant.h"
#include "ZipInternal.h"


using namespace hnrt;


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

        String pszPath = Path::Combine(pszOutFolder, m_bstrName);
        callbacks.ZipExtractOnStart(pszPath, m_Size);
        Variant item(VT_DISPATCH, m_pFolderItem);
        Variant options(VT_I4, FOF_NO_UI);
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
