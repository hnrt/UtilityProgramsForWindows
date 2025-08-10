#include "pch.h"
#include "hnrt/FolderItemsPtr.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


FolderItemsPtr::~FolderItemsPtr()
{
    Interlocked<FolderItems*>::ExchangePointer(&m_ptr, NULL)->Release();
}
