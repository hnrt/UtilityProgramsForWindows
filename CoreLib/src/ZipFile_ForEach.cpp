#include "pch.h"
#include "hnrt/ZipFile.h"
#include "hnrt/FolderItemsPtr.h"


using namespace hnrt;


BOOL ZipFile::ForEach(IFolderItemForEachCallback& callback)
{
    FolderItemsPtr pFolderItems = FolderItemsPtr::Get(m_pFolder);
    long count = pFolderItems.Count;
    for (long index = 0L; index < count; index++)
    {
        FolderItemPtr pFolderItem = FolderItemPtr::Get(index, pFolderItems);
        if (!callback.ForEach(pFolderItem))
        {
            return FALSE;
        }
    }
    return TRUE;
}
