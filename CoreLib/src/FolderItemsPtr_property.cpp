#include "pch.h"
#include "hnrt/FolderItemsPtr.h"
#include "hnrt/ComException.h"


using namespace hnrt;


long FolderItemsPtr::get_Count() const
{
    long count = 0L;
    HRESULT hr = m_ptr->get_Count(&count);
    if (hr == S_OK)
    {
        return count;
    }
    else
    {
        throw ComException(hr, L"FolderItems::get_Count failed.");
    }
}
