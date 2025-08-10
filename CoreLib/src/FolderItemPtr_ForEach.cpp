#include "pch.h"
#include "hnrt/FolderItemPtr.h"
#include "hnrt/FolderPtr.h"
#include "hnrt/FolderItemsPtr.h"
#include "hnrt/ComException.h"


using namespace hnrt;


BOOL FolderItemPtr::ForEach(IFolderItemForEachCallback& callback)
{
    if (IsFolder)
    {
        FolderPtr pFolder = FolderPtr::Get(*this);
        FolderItemsPtr pFolderItems = FolderItemsPtr::Get(pFolder);
        long count = pFolderItems.Count;
        for (long index = 0L; index < count; index++)
        {
            FolderItemPtr pFolderItem = FolderItemPtr::Get(index, pFolderItems);
            if (!callback.ForEach(pFolderItem))
            {
                return FALSE;
            }
        }
    }
    else if (!callback.ForEach(*this))
    {
        return FALSE;
    }
    return TRUE;
}
