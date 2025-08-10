#include "pch.h"
#include <shellapi.h>
#include "hnrt/FolderItemPtr.h"
#include "hnrt/FolderPtr.h"
#include "hnrt/ShellDispatchPtr.h"
#include "hnrt/Path.h"
#include "hnrt/Variant.h"


using namespace hnrt;


void FolderItemPtr::CopyTo(PCWSTR pszDestination, IFolderItemProcessCallbacks& callbacks)
{
    if (IsFolder)
    {
        struct CopyToLocalContextForFolder
            : public IFolderItemForEachCallback
        {
            PCWSTR pszDestination;
            IFolderItemProcessCallbacks& callbacks;

            CopyToLocalContextForFolder(PCWSTR pszDestination_, IFolderItemProcessCallbacks& callbacks_)
                : pszDestination(pszDestination_)
                , callbacks(callbacks_)
            {
            }

            virtual BOOL ForEach(FolderItemPtr& pFolderItem)
            {
                pFolderItem.CopyTo(pszDestination, callbacks);
                return TRUE;
            }

        } ctx(pszDestination, callbacks);

        ForEach(ctx);
    }
    else
    {
        FolderPtr pFolder = FolderPtr::Get(pszDestination, ShellDispatchPtr::Create());
        String pszPath = Path::Combine(pszDestination, Name);
        callbacks.OnStart(pszPath, Size);
        Variant item(VT_DISPATCH, m_ptr);
        Variant options(VT_I4, FOF_NO_UI);
        HRESULT hr = pFolder->CopyHere(item, options);
        if (hr == S_OK)
        {
            callbacks.OnSuccess(pszPath);
        }
        else
        {
            callbacks.OnFailure(pszPath, hr);
        }
    }
}
