#include "pch.h"
#include "hnrt/ZipFile.h"
#include "hnrt/Path.h"


using namespace hnrt;


void ZipFile::ExtractFiles(PCWSTR pszFilePattern, PCWSTR pszDestination, IFolderItemProcessCallbacks& callbacks)
{
    struct LocalContext
        : public IFolderItemForEachCallback
    {
        PCWSTR pszFilePattern;
        String pszDestination;
        IFolderItemProcessCallbacks& callbacks;
        bool bExtract;

        LocalContext(PCWSTR pszFilePattern_, PCWSTR pszDestination_, IFolderItemProcessCallbacks& callbacks_)
            : pszFilePattern(pszFilePattern_)
            , pszDestination(pszDestination_)
            , callbacks(callbacks_)
            , bExtract(false)
        {
        }

        ~LocalContext() = default;

        virtual BOOL ForEach(FolderItemPtr& pFolderItem)
        {
            String pszName = pFolderItem.Name;
            if (pFolderItem.IsFolder)
            {
                bool bPrevious = bExtract;
                if (!bExtract && Path::WildcardMatch(pszFilePattern, pszName))
                {
                    bExtract = true;
                }
                String pszOriginal = pszDestination;
                pszDestination = Path::Combine(pszDestination, pszName);
                pFolderItem.ForEach(*this);
                pszDestination = pszOriginal;
                bExtract = bPrevious;
            }
            else if (bExtract || Path::WildcardMatch(pszFilePattern, pszName))
            {
                Path::CreateDirectory(pszDestination);
                pFolderItem.CopyTo(pszDestination, callbacks);
            }
            return TRUE;
        }

    } ctx(pszFilePattern, pszDestination, callbacks);

    ForEach(ctx);
}
