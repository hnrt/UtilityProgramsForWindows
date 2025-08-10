#include "pch.h"
#include "hnrt/FolderItemPtr.h"
#include "hnrt/Interlocked.h"


using namespace hnrt;


FolderItemPtr::~FolderItemPtr()
{
	Interlocked<FolderItem*>::ExchangePointer(&m_ptr, NULL)->Release();
}
